import json
import uuid
import hashlib
import argparse
import os
import shutil
from zipfile import ZipFile
import subprocess
import sys

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

HAT_BLOCKS = ['control_start_as_clone', 'event_whenflagclicked', 'event_whenkeypressed', 'event_whenthisspriteclicked', 'event_whentouchingobject', 'event_whenstageclicked', 'event_whenbackdropswitchesto', 'event_whengreaterthan', 'event_whenbroadcastreceived', 'procedures_definition']
C_BLOCKS = ['control_repeat', 'control_repeat_until', 'control_while', 'control_for_each', 'control_forever', 'control_if', 'control_if_else']

# these blocks can be converted into native operations, like x = 5 instead of data_setvariableto(x, 5)
SPECIAL_BLOCKS = ['data_variable', 'data_setvariableto', 'data_changevariableby', 'procedures_call', 'control_wait_until']
BLOCK_PREFIX = 'block::'
TS_PREFIX = 'ts::'

with open(os.path.dirname(os.path.abspath(__file__)) + '/opcodes.json') as f:
    opcodes = json.load(f)
    NORMAL_BLOCKS = opcodes['allowed']
    NOT_IMPLEMENTED_BLOCKS = opcodes['not implemented']
    DISALLOWED_BLOCKS = opcodes['disallowed']

LOOP_COUNTER = 0

IGNORE_NOT_IMPLEMENTED_BLOCKS = True

class Block:
    def __init__(self, data, sprite, warp_mode, id=None):
        self.opcode = data['opcode']
        if self.opcode in DISALLOWED_BLOCKS:
            raise NotImplementedError("This project contains an unsafe block and will not be compiled.")
        
        self.sprite = sprite
        
        self.warp_mode = warp_mode
        
        if self.opcode == 'procedures_definition':
            self.prototype = sprite['blocks'][data['inputs']['custom_block'][1]]
            self.mutation = self.prototype['mutation']
            if self.prototype['mutation']['warp']:
                self.warp_mode = True
        if self.opcode == 'procedures_call':
            self.mutation = data['mutation']

        if self.opcode in ['procedures_prototype', 'key_options']:
            return
        
        if id is not None:
            self.id = get_C_id(id)
        else:
            self.id = 'uuid_' + str(uuid.uuid1())
                
        self.next = None
        if data['next'] is not None:
            self.next = Block(sprite['blocks'][data['next']], sprite, self.warp_mode)
        self.substack1 = None
        self.substack2 = None
        self.condition = None
        self.variable = None
        self.list = None
        
        self.toplevel = data['topLevel']

        self.inputs = {}
        
        if 'SUBSTACK' in data['inputs']:
            self.substack1 = Block(sprite['blocks'][data['inputs']['SUBSTACK'][1]], sprite, self.warp_mode)
        if 'SUBSTACK2' in data['inputs']:
            self.substack2 = Block(sprite['blocks'][data['inputs']['SUBSTACK2'][1]], sprite, self.warp_mode)
        if 'CONDITION' in data['inputs']:
            self.condition = Block(sprite['blocks'][data['inputs']['CONDITION'][1]], sprite, self.warp_mode)
        if 'KEY_OPTION' in data['inputs']:
            self.inputs['key_option'] = ['string', sprite['blocks'][data['inputs']['KEY_OPTION'][1]]['fields']['KEY_OPTION'][0]]
        if 'COLOR_PARAM' in data['inputs']:
            self.inputs['color_param'] = ['string', sprite['blocks'][data['inputs']['COLOR_PARAM'][1]]['fields']['colorParam'][0]]
        if 'INDEX' in data['inputs'] and data['inputs']['INDEX'][1][1] == 'all':
            self.inputs['foo'] = ['string', 'all'] # for some reason this doesn't do anything. TODO investigate
            data['inputs'].pop('INDEX')
        if 'VARIABLE' in data['fields'] and self.opcode in ['data_showvariable', 'data_hidevariable']:
            self.inputs['variable'] = ['variable', data['fields']['VARIABLE'][1]]
        elif 'VARIABLE' in data['fields']:
            self.variable = data['fields']['VARIABLE'][1]
        if 'OPERATOR' in data['fields']:
            self.inputs['operator'] = ['string', data['fields']['OPERATOR'][0]]
        if 'TO' in data['fields']:
            self.inputs['to'] = ['string', data['fields']['TO'][0]]
        if 'VALUE' in data['fields']:
            self.inputs['value'] = ['argument', data['fields']['VALUE'][0]]
        if 'COSTUME' in data['fields']:
            self.inputs['costume'] = ['string', data['fields']['COSTUME'][0]]
        if 'STOP_OPTION' in data['fields']:
            if data['fields']['STOP_OPTION'][0] not in ['all', 'this script']:
                if IGNORE_NOT_IMPLEMENTED_BLOCKS:
                    log.warn(data['fields']['STOP_OPTION'][0] + " has not been implemented for control_stop")
                else:
                    raise NotImplementedError(data['fields']['STOP_OPTION'][0] + " has not been implemented for control_stop")
            self.inputs['stop_option'] = ['string', data['fields']['STOP_OPTION'][0]]
            
        for input in data['inputs']:
            if input in ['SUBSTACK', 'SUBSTACK2', 'CONDITION', 'KEY_OPTION', 'COLOR_PARAM']: continue
            
            self.inputs[input] = get_input(data['inputs'][input], sprite)
            
        if 'LIST' in data['fields']:
            self.inputs['LIST'] = ['variable', data['fields']['LIST'][1]]
    
    def toC(self, level=0) -> str:
        global LOOP_COUNTER
        
        disable_warpmode = False
        
        indent = '    ' * level
        if self.opcode == 'procedures_definition':
            out = 'void ' + get_C_id(self.mutation['proccode']) + f'({TS_PREFIX}sprite* s, '
            for id in json.loads(self.mutation['argumentnames']):
                out += f'{TS_PREFIX}value {get_C_id(id)}, '
            out = out[:-2] # remove trailing comma
            out += ') {\n'
            if self.warp_mode:
                out += indent + f'\t++{TS_PREFIX}warp_mode;\n'
                disable_warpmode = True
            
            out += '\ttry {\n'
        elif self.opcode.startswith('argument_reporter_'):
            return input_to_C(self.inputs['value'])
        elif self.opcode in HAT_BLOCKS:
            out = f'''ts::Thread {self.id}({TS_PREFIX}Sprite* s) {{\n'''
        elif self.opcode in C_BLOCKS:
            out = '' + indent
            if self.opcode == 'control_repeat':
                out += f'for (int i{LOOP_COUNTER} = 0; i{LOOP_COUNTER} < {input_to_C(self.inputs["TIMES"])}; i{LOOP_COUNTER}++) {{'
                LOOP_COUNTER += 1
            elif self.opcode == 'control_repeat_until':
                out += f'while (!{str(self.condition)}.get_d()) {{'
            elif self.opcode == 'control_while':
                out += f'while ({str(self.condition)}.get_d()) {{'
            elif self.opcode == 'control_forever':
                out += 'while (true) {'
            elif self.opcode in ['control_if', 'control_if_else']:
                out += f'if ({str(self.condition)}.get_d()) {{'
            elif self.opcode == 'control_for_each':
                out += f'for (int i{LOOP_COUNTER} = 1; i{LOOP_COUNTER} <= {input_to_C(self.inputs["VALUE"])}; i{LOOP_COUNTER}++) {{'
            
            out += '\n'
            if self.substack1 is not None:
                out += self.substack1.toC(level+1)
            
            if self.substack2 is not None:
                out += indent + '} else {\n'
                out += self.substack2.toC(level+1)
            
            # insert screen refresh in loops
            if self.opcode in ['control_repeat', 'control_repeat_until', 'control_while', 'control_forever', 'control_for_each']:
                out += indent + f'\t{screen_update()}'
                
            out += indent + '}\n'
        elif self.opcode in SPECIAL_BLOCKS:
            out = ''
            match self.opcode:
                case 'data_variable':
                    if IGNORE_NOT_IMPLEMENTED_BLOCKS:
                        log.warn(f'{self.opcode} has not been implemented. See the implementation_notes.md file for more information.')
                    else:
                        raise NotImplementedError(f'{self.opcode} has not been implemented. See the implementation_notes.md file for more information.')
                case 'data_setvariableto': # TODO change these to #defines
                    out = indent + f'{get_C_id(self.variable)} = {input_to_C(self.inputs["VALUE"])};\n'
                case 'data_changevariableby':
                    out = indent + f'{get_C_id(self.variable)} += {input_to_C(self.inputs["VALUE"])};\n'
                case 'procedures_call':
                    out = indent + f'{get_C_id(self.mutation["proccode"])} (s,'
                    for input in json.loads(self.mutation['argumentids']):
                        if input in self.inputs.keys():
                            out += input_to_C(self.inputs[input]) + ','
                        else:
                            out += f'{TS_PREFIX}value(""),'
                    out = out[:-1] # get rid of trailing comma
                    out += ');\n'
                case 'control_wait_until':
                    out += indent + f'while ({str(self.condition)}) {{ {screen_update()} }}'
        else:
            if self.opcode not in NORMAL_BLOCKS:
                if IGNORE_NOT_IMPLEMENTED_BLOCKS:
                    log.warn(f'{self.opcode} has not been implemented. See the implementation_notes.md file for more information.')
                else:
                    raise NotImplementedError(f'{self.opcode} has not been implemented. See the implementation_notes.md file for more information.')
            
            out = indent + BLOCK_PREFIX + self.opcode
            
            out += '(s, '
            for input in self.inputs.values():
                out += input_to_C(input)
                out += ', '
            out = out[:-2] # remove trailing comma
            out += ')'
            if level != -1: out += ';\n'
            
        if self.next is not None:
            if self.opcode in HAT_BLOCKS:
                out += self.next.toC(2)
            else:
                out += self.next.toC(level)
        
        if self.opcode in HAT_BLOCKS:
            out += '\t} catch (std::runtime_error e) {}\n'
            if disable_warpmode:
                out += indent + f'\t--{TS_PREFIX}warp_mode;\n'
            
            if not self.opcode == 'procedures_definition':
                out += f'''\t--{TS_PREFIX}current_waiters[caller_id];
    if ({TS_PREFIX}current_waiters[caller_id] == 0) {TS_PREFIX}current_waiters.erase(caller_id);\n'''
            out += '}\n'
        return out
    
    def __str__(self) -> str:
        return self.toC(-1)

def setup(project):
    out = 'void setup() {\n'
    vars = ''
    prejs = 'var assetNames = ['
    preprocessor = f'''\
#define TS_PREFIX {TS_PREFIX}
#define BLOCK_PREFIX {BLOCK_PREFIX}
#include "threespeed.h"
 
namespace block = ts::block;
'''

    for target in project['targets']:
        for variable in target['variables']:
            vars += f'{TS_PREFIX}value {get_C_id(variable)}("{escape_string(target["variables"][variable][1])}");\n'
        for l in target['lists']:
            vars += f'{TS_PREFIX}list {get_C_id(l)};\n'
        vars += f'{TS_PREFIX}Sprite {get_C_id(target["name"])};\n'
        
        out += f'    {TS_PREFIX}sprites.push_back(&{get_C_id(target["name"])});\n'
        if not target['isStage']:
            out += f'    {get_C_id(target["name"])}.x = {target["x"]};\n'
            out += f'    {get_C_id(target["name"])}.y = {target["x"]};\n'
            out += f'    {get_C_id(target["name"])}.direction = {target["direction"]-90};\n'
        
        for costume in target['costumes']:
            out += f'    {get_C_id(target["name"])}.load_costume("{costume["md5ext"]}", "{costume["name"]}", {costume["rotationCenterX"]}, {costume["rotationCenterX"]});\n'
            prejs += f'"{costume["md5ext"]}",'
            
        # custom block declarations
        for b in get_blockstacks(project):
            if b.opcode == 'procedures_definition':
                declaration = 'void ' + get_C_id(b.mutation['proccode']) + f'({TS_PREFIX}sprite* s, '
                for id in json.loads(b.mutation['argumentnames']):
                    declaration += f'{TS_PREFIX}value {get_C_id(id)}, '
                declaration = declaration[:-2] # remove trailing comma
                declaration += ');\n'
                vars += declaration

        # hat blocks
        for b in target['blocks']:
            if type(target['blocks'][b]) == type([]):
                continue
            if target['blocks'][b]['topLevel'] and target['blocks'][b]['opcode'] in HAT_BLOCKS and target['blocks'][b]['opcode'] != 'procedures_definition':
                block = target['blocks'][b]
                event = block['opcode']
                if block['opcode'] == 'event_whenkeypressed':
                    if 'arrow' in block['fields']['KEY_OPTION'][0]:
                        event = 'event_whenkeypressed_' + block['fields']['KEY_OPTION'][0].replace(' arrow', '')
                    else:
                        event = 'event_whenkeypressed_' + block['fields']['KEY_OPTION'][0].replace('\\', '\\\\')
                if block['opcode'] == 'event_whenbroadcastreceived':
                    event = 'event_whenbroadcastreceived_' + get_C_id(block['fields']['BROADCAST_OPTION'][1])

                event_uname = event + '_' + get_C_id(b)

                out += f'    {get_C_id(target["name"])}.register_event("{event}", "{event_uname}", &{get_C_id(b)});\n'
    
    for monitor in project['monitors']:
        if monitor['mode'] == 'list':
            #continue # TODO implement list monitors
            pass
        if len(monitor['params'].values()) > 0:
            # built in variables like loudness
            # TODO don't skip these
            variable_name = list(monitor["params"].values())[0]
            out += f'\t{TS_PREFIX}{"variable_monitors" if monitor["mode"] != "list" else "list_monitors"}.emplace(&{get_C_id(monitor["id"])}, {TS_PREFIX}monitor_t{{true, "{variable_name}", {monitor["x"]}, {monitor["y"]}, {monitor["width"]}, {monitor["height"]}}});\n' # TODO is it shown?

    prejs += '];\n'
    prejs += ''

    out += '}'

    out += '''
int main() {
    ts::init();
    setup();
    ts_main();

    for (int i = 0; i < ts::threads.size(); i++) {
        ts::threads[i].detach();
    }
}'''
    
    return out, vars, preprocessor, prejs

c_ids = {}
c_id = 0

def get_C_id(id):
    global c_ids
    global c_id
    hash = hashlib.sha256(id.encode()).hexdigest()
    if hash in c_ids.keys():
        return 'id_' + str(c_ids[hash])
    else:
        c_id += 1
        c_ids[hash] = c_id
        return 'id_' + str(c_ids[hash])

def escape_string(string):
    if type(string) == int or type(string) == float or type(string) == bool:
        return string
    out = ''
    for s in string:
        out += '\\x' + hex(ord(s))[2:]
    return out

def input_to_C(input):
    if input[0] == 'string':
        try:
            if str(float(input[1])) == 'nan':
                return f'{TS_PREFIX}value(std::numeric_limits<double>::quiet_NaN())'
            else:
                return f'{TS_PREFIX}value({float(input[1])})'
        except:
            return f'{TS_PREFIX}value("{escape_string(input[1])}")'
    elif input[0] == 'color':
        return f'{TS_PREFIX}value(0x{input[1]})'
    elif input[0] == 'number':
        return f'{TS_PREFIX}value({input[1]})'
    elif input[0] == 'id': # this may need to change in the future
        return f'{TS_PREFIX}value("{input[1]}")'
    elif input[0] == 'variable':
        return f'{get_C_id(input[1])}'
    elif input[0] == 'list':
        return f'{get_C_id(input[1])}'
    elif input[0] == 'variable':
        return get_C_id(input[1])
    elif input[0] == 'block':
        return str(input[1])
    elif input[0] == 'argument':
        return get_C_id(input[1])
    else:
        print('what')

# Note on shadows: the second value of the input is always the real value, no matter the shadow state
# gets the actual value of an input
def get_input(input, sprite):
    if type(input[1]) == str:
        # id to another block
        if sprite['blocks'][input[1]]['opcode'].startswith('argument_reporter_'):
            return ['argument', sprite['blocks'][input[1]]['fields']['VALUE'][0]]
        return ['block', Block(sprite['blocks'][input[1]], sprite, True)] # nothing in an input should care about warp mode, but it makes more sense for it to be on if that ever happens
    elif type(input[1]) == list:
        # (type, value) input
        if input[1][0] >= 4 and input[1][0] <= 8: # number or angle
            if input[1][1] == '':
                return ['number', 0]
            if input[1][1] == 'nan':
                return ['number', 'std::nan']
            return ['number', input[1][1]]
        elif input[1][0] == 9: # color
            return ['color', input[1][1][1:]]
        elif input[1][0] == 10: # string
            return ['string', input[1][1]]
        elif input[1][0] == 11: # broadcast
            return ['id', get_C_id(input[1][2])]
        elif input[1][0] == 12: # variable
            return ['variable', input[1][2]]
        elif input[1][0] == 13: # list
            return ['list', input[1][2]]
        else:
            raise Exception("Malformed project.json: invalid input type '" + str(input) + "'")
    elif input[1] == None:
        return ['string', '']
    else:
        raise Exception("Malformed project.json: invalid input '" + str(input) + "'")

# returns the code for a screen update
def screen_update():
    return f"{TS_PREFIX}screen_update();\n"
    
def get_blockstacks(project):
    stacks = []
    
    # find hat blocks
    for sprite in project['targets']:
        for b in sprite['blocks']:
            block = sprite['blocks'][b]
            if type(block) == list:
                continue
            if block['topLevel'] and block['opcode'] in HAT_BLOCKS:
                stacks.append(Block(block, sprite, False, id=b)) # assume warp mode is off until we look at the prototype

    return stacks

def generate(project):
    setup_function, var, preprocessor, prejs = setup(project)

    out = f'''{preprocessor}
{var}
'''
              
    stacks = get_blockstacks(project)
    
    for stack in stacks:
        out += str(stack) + '\n'
        
    out += '\n' + setup_function
    
    return out
    
    # this is for emscripten, which isn't supported yet
    #with open('pre.js', 'w') as f:
    #    f.write(prejs)


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
    print('You must specify a project when generating code!')
    quit(1)

if not args.no_generate:
    log.info("extracting")
    if os.path.exists(args.output):
        shutil.rmtree(args.output)
    os.mkdir(args.output)
    ZipFile(args.project).extractall(args.output)

    project = open(args.output + '/project.json')
    log.info("generating")
    output = generate(json.load(project))
    log.info("writing")
    output_file = open(args.output + '/generated.cpp', 'w')
    output_file.write(output)
    output_file.close()
    project.close()
    log.info("done")

if not args.no_compile:
    if args.run:
        subprocess.run(['make', 'run', 'SOURCE=' + args.output])
    else:
        subprocess.run(['make', 'SOURCE=' + args.output])