import argparse
import os
import shutil
import subprocess
from zipfile import ZipFile
import json
import hashlib
import re

class Logger:
    def __init__(self):
        pass
    
    def log(self, level, message):
        print(f'{level} {message}\n', end='')
    
    def debug(self, message):
        self.log('\x1B[32m[DEBUG]\033[0m', message)
    
    def info(self, message):
        self.log('[INFO] ', message)
    
    def warn(self, message):
        self.log('\x1B[33m[WARN]\033[0m ', message)
    
    def error(self, message):
        self.log('\x1B[31m[ERROR]\033[0m', message)

log = Logger()

seen_opcodes = set()

custom_blocks = set()

# variable id -> sprite containing variable
personal_variables = {}

sprite_name = " INVALID "

'''Escape a value so it is safe to insert directly into the generated code.'''
def escape_string(string):
    if type(string) == int or type(string) == float:
        return string
    if type(string) == bool:
        return "true" if string else "false"
    out = '"'
    for s in string:
        out += '\\u' + str(hex(ord(s))[2:]).zfill(4)
    return out + '"'

'''Convert a scratch id into a string safe to use as an identifier'''
def make_ts_id(scratch_id, sprite_id = ""):
    hash = hashlib.sha256(scratch_id.encode()).hexdigest()
    scratch_id = re.sub(r'[^a-zA-Z0-9]+', '_', scratch_id)
    spriteHash = hashlib.sha256(sprite_id.encode()).hexdigest()
    return f'id_{hash[0:8]}_{scratch_id}'
    return 'id_' + hash # TODO: make the ids shorter

def make_block(id: str, parent, all_blocks, warp=False):
    if id is None:
        log.debug(f"child of {parent.id} does not exist")
        return EmptyBlock()
    if all_blocks[id]['opcode'] in ['control_repeat', 'control_repeat_until', 'control_while', 'control_for_each', 'control_forever', 'control_if', 'control_if_else', 'control_all_at_once']:
        return CBlock(id, parent, all_blocks, warp)
    elif all_blocks[id]['opcode'] == 'procedures_call':
        return CustomBlockCall(id, parent, all_blocks, warp)
    else:
        return StackBlock(id, parent, all_blocks, warp)

def make_personal_id(sprite, variable, external: bool = False, sprite_id: str = ''):
    if external:
        return make_ts_id(variable, sprite["name"])
    if sprite_id == '': sprite_id = 'ts::current_sprite'
    return f'(((variables_{make_ts_id(sprite["name"])}*) ({sprite_id}->personal_variables))->{make_ts_id(variable, sprite["name"])})'

class Input:
    def __init__(self, source, all_blocks, warp=False):
        if source is None:
            self.type = None
            self.data = ''
            return
        
        elif type(source[1]) == str: # id to another block
            self.type = 'block_id'
            self.data = InputBlock(source[1], self, all_blocks, warp)
        elif type(source[1]) == list: # primitive or variable
            self.data = source[1][1]
            
            if source[1][0] >= 4 and source[1][0] <= 8: # some form of number
                # TODO is there a difference between number types?
                if type(source[1][1]) == str: # list indices are always marked as a number but may be the string "all"
                    self.type = 'list_index'
                else:
                    self.type = 'number'
            elif source[1][0] == 9:
                self.type = 'color'
            elif source[1][0] == 10: # string
                if type(source[1][1]) == str:
                    try:
                        val = float(source[1][1])
                    except ValueError:
                        self.type = 'string'
                    else:
                        if source[1][1] == str(val):
                            self.type = 'number'
                        else:
                            self.type = 'string'
                elif type(source[1][1]) == float or type(source[1][1]) == int:
                    self.type = 'number'
                else:
                    raise Exception('project.json is invalid')
            elif source[1][0] == 11:
                self.type = 'broadcast'
                self.data = source[1][1]
            elif source[1][0] == 12:
                self.type = 'variable'
                self.data = source[1][2] # TODO is the name relevant in any way?
            elif source[1][0] == 13:
                self.type = 'list'
                self.data = source[1][2]
            else:
                raise Exception("project.json is invalid")
        elif source[1] is None:
            self.type = 'undefined'
            self.data = ''
        else:
            raise Exception("project.json is invalid")
        
        self.sprite_id = sprite_name

    def serialize(self):
        match self.type:
            case 'number':
                return f'ts::Number({self.data})'
            case 'color': # TODO add actual color type
                return f'ts::String({escape_string(self.data)})'
            case 'string' | 'list_index': # TODO add actual list index type
                return f'ts::String({str(escape_string(self.data))})'
            case 'broadcast':
                return f'std::string({str(escape_string(self.data))})'
            case 'variable' | 'list':
                if self.data in personal_variables:
                    return make_personal_id(personal_variables[self.data], self.data)
                return make_ts_id(self.data, self.sprite_id)
            case 'block_id':
                return self.data.serialize()
            case 'undefined':
                return 'ts::String("")'
            case _:
                raise Exception(f"invalid input type ({repr(self.type)})")

class Field(Input):
    def __init__(self, source, all_blocks):
        if len(source) == 1:
            self.type = 'not_id'
            self.data = source[0]
        elif len(source) == 2:
            if source[1] is None:
                self.type = 'not_id'
                self.data = source[0]
            else:
                self.type = 'id'
                self.data = source[1]
                self.name = source[0]
        else:
            raise Exception("project.json is invalid")
    
    def serialize(self):
        if self.data is None: # I'm not sure why this happens or what it means. TODO
            return f'ts::String("")'
        if self.type == 'not_id':
            return f'ts::String({escape_string(self.data)})'
        else:
            if self.data in personal_variables.keys():
                return make_personal_id(personal_variables[self.data], self.data)
            return make_ts_id(self.data)

class Block:
    def __init__(self, id, parent, all_blocks, warp=False):
        source = all_blocks[id]
        
        seen_opcodes.add(source['opcode'])
        
        self.id = id
        self.opcode = source['opcode']
        if source['next']:
            self.next = make_block(source['next'], self, all_blocks, warp)
        else:
            self.next = None
        self.parent = parent
        
        self.warp = warp

        self.sprite_id = sprite_name
    
    def serialize(self):
        out = f'ts::block::{self.opcode}('
        for input in self.inputs:
            out += self.inputs[input].serialize() + ', '
        if len(self.inputs) > 0:
            out = out[:-2] # remove trailing comma
        out += ')'
        
        return out

class EmptyBlock(Block):
    def __init__(self):
        pass

    def serialize(self):
        return "/* empty block */"

class InputBlock(Block):
    def __init__(self, id, parent, all_blocks, warp=False):
        super().__init__(id, parent, all_blocks, warp)
        
        source = all_blocks[id]
        self.inputs = {}
        for input in source['inputs']:
            self.inputs[input] = Input(source['inputs'][input], all_blocks, warp)
        for field in source['fields']:
            self.inputs[field] = Field(source['fields'][field], all_blocks)
        
        if 'mutation' in source.keys():
            self.mutation = source['mutation']
    
    def serialize(self):
        if self.opcode == 'argument_reporter_boolean' and self.inputs['VALUE'].data == 'is compiled?':
            return 'true'
        if self.opcode == 'argument_reporter_string_number' or self.opcode == 'argument_reporter_boolean':
            return make_ts_id(self.inputs['VALUE'].data)
        if self.opcode == 'sensing_of_object_menu':
            return make_ts_id(self.inputs['OBJECT'].data)
        if self.opcode == 'motion_goto_menu':
            return make_ts_id(self.inputs['TO'].data)
        else:
            return super().serialize()

class StackBlock(Block):
    def __init__(self, id, parent, all_blocks, warp=False):
        super().__init__(id, parent, all_blocks, warp)
        
        source = all_blocks[id]
        self.inputs = {}
        for input in source['inputs']:
            self.inputs[input] = Input(source['inputs'][input], all_blocks, warp)
        for field in source['fields']:
            self.inputs[field] = Field(source['fields'][field], all_blocks)
            
    def serialize(self):
        if self.opcode == 'control_stop':
            match self.inputs['STOP_OPTION'].data:
                case 'this script':
                    if self.warp:
                        out = 'return;\n'
                    else:
                        out = 'co_return {ts::thread_signal::EXIT};\n'
                case 'all':
                    out = ''
                    # TODO
                    pass
                case 'other scripts in sprite':
                    out = ''
                    # TODO
                    pass
                case _:
                    raise Exception(f"invalid option for control_stop ({self.inputs['STOP_OPTION'].data})")
        elif self.opcode == 'data_showvariable' or self.opcode == 'data_hidevariable':
            out = f'ts::block::{self.opcode}({self.inputs['VARIABLE'].serialize()}_monitor);\n'
        elif self.opcode == 'control_wait':
            out = f'CUSTOM_CALL(ts::block::control_wait({self.inputs['DURATION'].serialize()}));\n'
        elif self.opcode == 'control_wait_until':
            if self.warp:
                out = f'CONTROL_WAIT_UNTIL_WARP({self.inputs['CONDITION'].serialize()});\n'
            else:
                out = f'CONTROL_WAIT_UNTIL({self.inputs['CONDITION'].serialize()});\n'
        elif self.opcode == 'control_delete_this_clone':
            out = f'ts::delete_personal_vars<variables_{make_ts_id(sprite_name)}>(ts::current_sprite);\n'
            out += 'co_return {ts::thread_signal::EXIT | ts::thread_signal::DELETE};\n'
        elif self.opcode == 'control_create_clone_of':
            out = f'ts::block::control_create_clone_of<variables_{make_ts_id(sprite_name)}>({self.inputs['CLONE_OPTION'].serialize()});\n'
        elif self.opcode == 'event_broadcastandwait':
            if self.warp:
                # TODO implement broadcastandwait in warping custom blocks
                # this will broadcast but not wait
                out = f'ts::block::event_broadcast({self.inputs['BROADCAST_INPUT'].serialize()});\n'
            else:
                out = f'CUSTOM_CALL(ts::block::event_broadcastandwait({self.inputs['BROADCAST_INPUT'].serialize()}));\n'
        elif self.opcode == 'sound_playuntildone':
            out = f'CUSTOM_CALL(ts::block::sound_playuntildone({self.inputs['SOUND_MENU'].serialize()}));\n'
        else:
            out = super().serialize() + ';\n'
        if self.next is not None:
            out += self.next.serialize()
        return out
    
class CustomBlockCall(Block):
    def __init__(self, id, parent, all_blocks, warp=False):
        super().__init__(id, parent, all_blocks, warp)
        
        source = all_blocks[id]
        self.inputs = {}
        for input in source['inputs']:
            self.inputs[input] = Input(source['inputs'][input], all_blocks, warp)
        for field in source['fields']:
            self.inputs[field] = Field(source['fields'][field], all_blocks)
            
        self.arguments = json.loads(source['mutation']['argumentids'])
        self.mutation = source['mutation']
        
        self.warp = self.warp or self.mutation['warp'] == 'true'
        
        self.all_blocks = all_blocks
            
    def serialize(self):
        if self.mutation['proccode'] not in custom_blocks:
            out = ''
            if self.mutation['proccode'] == 'â€‹â€‹logâ€‹â€‹ %s':
                out = f'LOG_INFO({self.inputs["arg0"].serialize()});\n'
            if self.mutation['proccode'] == 'â€‹â€‹warnâ€‹â€‹ %s':
                out = f'LOG_WARN({self.inputs["arg0"].serialize()});\n'
            if self.mutation['proccode'] == 'â€‹â€‹errorâ€‹â€‹ %s':
                out = f'LOG_ERROR({self.inputs["arg0"].serialize()});\n'

            if self.next is not None:
                out += self.next.serialize()
            
            return out
        
        if self.warp:
            out = f'{make_ts_id(f"{list(self.all_blocks.keys())[0]}__{self.mutation['proccode']}")}_warp('
        else:
            # out = f'CUSTOM_CALL({make_ts_id(self.mutation['proccode'])}('
            out = f'CUSTOM_CALL({make_ts_id(f"{list(self.all_blocks.keys())[0]}__{self.mutation['proccode']}")}('
        
        if len(self.arguments) > 0:
            for arg in self.arguments:
                if arg in self.inputs:
                    out += f'{self.inputs[arg].serialize()}, '
                else:
                    out += 'ts::String(""), '
            out = out[:-2]
        
        out += ');'
        if not self.warp:
            out += ')'
        out += '\n'
        
        if self.next is not None:
            out += self.next.serialize()
        
        return out

class HatBlock(StackBlock):
    def serialize(self):
        out = f'ts::Thread {make_ts_id(self.id)}() {{\n'
        if self.next is not None:
            out += self.next.serialize()
        out += 'co_return {ts::thread_signal::EXIT};\n}\n\n'
        
        return out

class CustomBlock(StackBlock):
    def __init__(self, id, parent, all_blocks, warp=False):
        super().__init__(id, parent, all_blocks, warp)
        
        self.arguments = json.loads(self.inputs['custom_block'].data.mutation['argumentnames'])
        proccode = self.inputs['custom_block'].data.mutation['proccode']
        self.argtypes = []
        for i in range(len(proccode)):
            if proccode[i] == '%':
                if i == len(proccode) - 1:
                    continue
                if proccode[i + 1] in ['s', 'b', 'n']:
                    self.argtypes.append(proccode[i + 1])
                else:
                    log.warn(f"unknown arg type in {self.id}: {proccode[i + 1]}")

        self.mutation = self.inputs['custom_block'].data.mutation
        self.warp = warp
        self.all_blocks = all_blocks
    
    def serialize(self, body = True):
        if self.warp:
            out = f'void {make_ts_id(f"{list(self.all_blocks.keys())[0]}__{self.mutation['proccode']}")}_warp('
        else:
            out = f'ts::Thread {make_ts_id(f"{list(self.all_blocks.keys())[0]}__{self.mutation['proccode']}")}('
            
        for i in range(len(self.arguments)):
            match self.argtypes[i]:
                case 's' | 'n': # TODO is 'n' different from 's'?
                    out += f'ts::Value {make_ts_id(self.arguments[i])}, '
                case 'b':
                    out += f'ts::Boolean {make_ts_id(self.arguments[i])}, '
        if len(self.arguments) > 0:
            out = out[:-2]
        
        if body:
            out += ') {\n'
            if self.next is not None:
                out += self.next.serialize()
            if not self.warp:
                out += 'co_return {ts::thread_signal::EXIT};\n'
            out += '}\n\n'
        else:
            out += ')'
            
        return out

class CBlock(Block):
    def __init__(self, id, parent, all_blocks, warp=False):
        super().__init__(id, parent, all_blocks, warp)
        
        source = all_blocks[id]
        self.inputs = {}
        for input in source['inputs']:
            if 'SUBSTACK' in input:
                id = source['inputs'][input][1]
                self.inputs[input] = make_block(id, self, all_blocks, warp)
            else:
                self.inputs[input] = Input(source['inputs'][input], all_blocks, warp)
        for field in source['fields']:
            self.inputs[field] = Field(source['fields'][field], all_blocks)
    
    def serialize(self):
        out = ''
        match self.opcode:
            case 'control_if' | 'control_if_else':
                if 'CONDITION' in self.inputs.keys():
                    out += f'if ({self.inputs['CONDITION'].serialize()}) {{\n'
                else:
                    out += 'if (false) {\n'
            case 'control_forever':
                out += 'while (true) {\n'
            case 'control_repeat':
                out += f'int max_{make_ts_id(self.id)} = {self.inputs["TIMES"].serialize()};\n'
                out += f'for (int i = 0; i < max_{make_ts_id(self.id)}; i++) {{\n'
            case 'control_for_each':
                out += f'for (int i = 0; i < (int) {self.inputs["VALUE"].serialize()}; i++) {{\n'
            case 'control_repeat_until':
                out += f'while (!(bool) {self.inputs["CONDITION"].serialize()}) {{\n'
            case 'control_while':
                out += f'while ((bool) {self.inputs["CONDITION"].serialize()}) {{\n'
            # TODO
            
            case _:
                raise Exception(f'invalid C block opcode {repr(self.opcode)}')

        for substack in ['SUBSTACK', 'SUBSTACK1']: # TODO fill out list
            if substack in self.inputs:
                out += self.inputs[substack].serialize()
        
        if not self.warp and self.opcode in ['control_while', 'control_repeat', 'control_for_each', 'control_forever', 'control_while']:
            out += 'co_yield {ts::thread_signal::YIELD};\n}\n'
        else:
            out += '}'
            
        
        if 'SUBSTACK2' in self.inputs:
            out += ' else {\n'
            out += self.inputs['SUBSTACK2'].serialize()
            out += '}'
        
        out += '\n'
        
        if self.next is not None:
            out += self.next.serialize()
        
        return out
    
def verify_asset_name(asset_name):
    parts = asset_name.split('.')
    if len(parts) != 2:
        return False
    if len(parts[0]) != 32:
        return False
    if len(parts[1]) != 3:
        return False
    
    for char in parts[0] + parts[1]:
        if char not in '0123456789abcdefsvgpngjpegwavmp3':
            return False
        
    return True
        
    

def prelude():
    return f'''#include "threespeed.h"\n
'''

def generate(project):
    global sprite_name
    pre = prelude()
    out = ''
    listdata = ''
    
    setup = "int main() {\nts::init();\n"
    
    for sprite in project['targets']:
        sprite_name = sprite["name"]
        sprite_id = make_ts_id(sprite_name)
        out += f'\n\n/***** {sprite_name} *****/\n\n'
        out += f'ts::Sprite *{sprite_id};\n'
        
        personal_variable_out = f'struct variables_{make_ts_id(sprite_name)} {{\n'
        for variable in sprite['variables']:
            if sprite_name == 'Stage':
                out += f'ts::Value {make_ts_id(variable, sprite_name)}({escape_string(sprite['variables'][variable][1])});\n'
            else: # handle 'for this sprite' variables
                personal_variable_out += f'ts::Value {make_personal_id(sprite, variable, external=True)};\n'
                personal_variables[variable] = sprite

        for l in sprite['lists']:
            listdata += str(len(sprite['lists'][l][1])) + '\x00'
            for item in sprite['lists'][l][1]:
                if type(item) == float:
                    listdata += 'f'
                elif type(item) == int:
                    listdata += 'i'
                else:
                    listdata += 's'
                listdata += str(item) + '\x00'
            
            if sprite_name == 'Stage':
                out += f'ts::List<ts::Value> {make_ts_id(l)};\n'
                setup += f'ts::load_list({make_ts_id(l)});\n'
            else: # handle 'for this sprite' lists
                personal_variable_out += f'ts::List<ts::Value> {make_personal_id(sprite, l, external=True)};\n'
                setup += f'ts::load_list({make_personal_id(sprite, l)});\n'
                personal_variables[l] = sprite
        personal_variable_out += '};\n'
        out += personal_variable_out
            
        out += f'variables_{make_ts_id(sprite_name)} variables_{make_ts_id(sprite_name)}_default;\n'
        setup += f'{sprite_id} = new ts::Sprite(ts::renderer, {escape_string(sprite_name)});\n'
        setup += f'ts::current_sprite = {sprite_id};\n'
        setup += f'{sprite_id}->personal_variables = &variables_{make_ts_id(sprite_name)}_default;\n'
        setup += f'{sprite_id}->personal_variables_size = sizeof(variables_{make_ts_id(sprite_name)});\n'
        setup += f'ts::register_sprite({sprite_id});\n'
        
        # check which custom blocks exist
        for block in sprite['blocks'].values():
            if type(block) == list:
                continue
            if block['opcode'] == 'procedures_definition':
                cb = sprite['blocks'][block['inputs']['custom_block'][1]]
                custom_blocks.add(cb['mutation']['proccode'])
        
        custom_block_defs = set()
        for block in sprite['blocks']:
            data = sprite['blocks'][block]
            if 'opcode' not in sprite['blocks'][block]:
                # Some reporters have a different json format. They can't be a hat block so we ignore them.
                continue
            
            if data['opcode'] == 'procedures_definition':
                # Custom blocks are generated twice. One version will
                # "run without screen refresh" (aka warp/turbo mode), the
                # other runs normally.
                cur_block = CustomBlock(block, None, sprite['blocks'], warp=True)
                decl = cur_block.serialize(False)
                if decl in custom_block_defs:
                    log.warn(f"Custom block was defined twice! id: {cur_block.id}")
                    continue
                custom_block_defs.add(decl)
                
                out += cur_block.serialize()
                pre += decl + ';\n'
                
                cur_block = CustomBlock(block, None, sprite['blocks'], warp=False)
                out += cur_block.serialize()
                pre += cur_block.serialize(False) + ';\n'
            elif data['opcode'] in ['event_whenflagclicked', 'event_whenkeypressed', 'event_whenbroadcastreceived', 'control_start_as_clone']: # TODO complete this list
                cur_block = HatBlock(block, None, sprite['blocks'])
                out += cur_block.serialize()
                
                event = f'new ts::Event{{ts::EventType::{cur_block.opcode}, '
                if cur_block.opcode == 'event_whenkeypressed':
                    event += f'SDL_GetKeyFromName("{cur_block.inputs['KEY_OPTION'].data}")}}'
                elif cur_block.opcode == 'event_whenbroadcastreceived':
                    event += f'{str(escape_string(cur_block.inputs['BROADCAST_OPTION'].name))}}}'
                elif cur_block.opcode == 'control_start_as_clone':
                    event += '0}'
                else:
                    event += '0}'
                setup += f'ts::register_handler({{{event}, {sprite_id}, {make_ts_id(cur_block.id)}, nullptr}});\n'
        
        for costume in sprite['costumes']:
            if verify_asset_name(costume["md5ext"]):
                setup += f'{sprite_id}->load_costume("{costume["md5ext"]}", {escape_string(costume["name"])}, {escape_string(costume["rotationCenterX"])}, {escape_string(costume["rotationCenterY"])});\n' # TODO rotation offset
            else:
                raise Exception(f'Costume md5ext is invalid ("{repr(costume["md5ext"])}")')
        for sound in sprite['sounds']:
            if verify_asset_name(sound["md5ext"]):
                setup += f'{sprite_id}->load_sound("{sound["md5ext"]}", {escape_string(sound["name"])});\n'
            else:
                raise Exception(f'Sound md5ext is invalid ("{repr(sound["md5ext"])}")')
            
    for monitor in project['monitors']:
        # built in variables like loudness have no params
        # TODO don't skip these
        if len(monitor['params'].values()) > 0:
            name = list(monitor["params"].values())[0]
            if monitor['mode'] == 'list':
                pass # TODO
            else:
                # TODO is it shown?
                sprite_name = monitor['spriteName'] if monitor['spriteName'] else "Stage"
                if monitor['id'] in personal_variables:
                    monitor_id = make_personal_id(personal_variables[monitor['id']], monitor['id'], sprite_id=make_ts_id(sprite_name))
                else:
                    monitor_id = make_ts_id(monitor['id'], sprite_name)
                setup += f'ts::variable_monitors.emplace_back(new ts::VariableMonitor{{{monitor['x']}, {monitor['y']}, {'true' if monitor['visible'] else 'false'}, {escape_string(name)}, &{monitor_id}}});\n'
                pre += f'ts::VariableMonitor *{make_ts_id(monitor['id'], sprite_name)}_monitor;\n'
                setup += f'{make_ts_id(monitor['id'], sprite_name)}_monitor = ts::variable_monitors.at(ts::variable_monitors.size() - 1);\n' # TODO I think this will break if elements are inserted in the middle
    
    setup += 'ts::start_event(new ts::Event{ts::EventType::event_whenflagclicked, 0});\nts::run();\n'
    setup += '}'
        
    
    return (pre + out + setup, listdata)
    

parser = argparse.ArgumentParser(
    prog='ThreeSpeed',
    description='Scratch to C++ compiler',
)
parser.add_argument('-p', '--project', help='An sb3 or zip file containing a scratch project')
parser.add_argument('-o', '--output', help='Directory to store output in', required=True)
parser.add_argument('--no-compile', help='Generate the project files but don\'t compile the generated C++ code', action='store_true')
parser.add_argument('--no-generate', help='Compile existing project files. Useful for manually editing a generated project.', action='store_true')
parser.add_argument('--run', help='Start the project after compiling', action='store_true') # todo dont compile

args = parser.parse_args()

if not args.no_generate and not args.project:
    log.error('You must specify a project when generating code!')
    quit(1)

if not args.no_generate:
    log.info(f"Extracting to {args.output}")
    if os.path.exists(args.output):
        shutil.rmtree(args.output)
    os.mkdir(args.output)
    ZipFile(args.project).extractall(args.output)

    project = open(args.output + '/project.json')
    log.info("Generating")
    output = generate(json.load(project))
    log.info("Writing")
    output_file = open(args.output + '/generated.cpp', 'w')
    output_file.write(output[0])
    output_file.close()
    with open(args.output + '/listdata', 'w') as list_file:
        list_file.write(output[1])
    project.close()
    log.info("done")

if not args.no_compile:
    if args.run:
        subprocess.run(['make', 'run', 'SOURCE=' + args.output])
    else:
        subprocess.run(['make', 'build', 'SOURCE=' + args.output])
