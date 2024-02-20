import math

def tryParseFloat(string, elseVal=None):
    try:return float(string)
    except:return elseVal
    
def tryEval(string, elseVal=None):
    try:return eval(string)
    except:return elseVal
  
def log(*v):
    #print('log:', *v)
    pass

def Upper(list):
    return [e.upper() for e in list]

def IS(e, l):
    return e.upper() in Upper(l)

class _():
    def __init__(self, *value):
        self.value = Upper(value)
    def __contains__(self, other):
        return other.upper() in self.value
    
OPER = {
    'SIMPLE': ['+', '-', '*', '/', '**', '//'],
    'SPECIA': {
        'ANTI': ['!','_'],
    },
    'LOGIC': ['==', '!=','and', 'or', '&', '|', '^', '<=', '>='],
    'SPEC_LOGIC': {
        '&&': 'and',
        '||': 'or',
        'XOR': '^',
        '<>': '!=',
        'IF': ' if ',
        'ELSE': ' else ',
        '===': '=='
    }
}
SPECIAL_OPERS = []
for k in OPER['SPECIA']:
    SPECIAL_OPERS += OPER['SPECIA'][k]
OPERS = OPER['SIMPLE'] + SPECIAL_OPERS + OPER['LOGIC']
OPERS += [k for k in OPER['SPEC_LOGIC']]

    
variables = {'PI':math.pi}
funcs = {'sin':{
    'function': lambda a: math.sin(float(a)),
    'params': ['a']
}}

def runFunc(codes,delta=0):
    func = funcs[codes[delta]]
    params_number = len(func['params'])
    params_value = []
    if 'equation' in func:
        equation = func['equation']
        for i in range(params_number):
            try:
                params_value += [codes[delta+1+i]]
            except:
                break
        for i in range(len(func['params'])):
            try:
                equation = equation.replace(localParm(func['params'][i]), params_value[i])
            except:
                break
        print(res := bafs_eval(equation.split(' ')))
        return res, len(func['params'])+1
    if 'function' in func:
        for i in range(params_number):
            try:
                params_value += [codes[delta+1+i]]
            except:
                break
        print(res := func['function'](*[bafs_eval([p], False) for p in params_value]))
        return res, len(func['params'])+1

def var_2_val(codes):
    i = -1
    while (i:=i+1) < len(codes):
        if codes[i] in variables:
            codes[i] = str(variables[codes[i]])
        elif codes[i] in funcs:
            val, jump = runFunc(codes, i)
            codes = codes[:i] + [str(val)] + codes[i+jump:]
    return codes

def bafs_eval(codes, printOut=True):
    res = var_2_val(codes)
    values = []
    opers = []
    value = 0
    indexOf = {'(':-1, ')':-1}
    _res = []

    def Print(*value):
        printOut and print(*value)

    def fix_bracket(start, rem):
        rem = [start] + rem
        indexOf = {'(':-1, ')':-1}
        if '(' in start:
            indexOf['('] = 0
        else:
            return start, 0
        i=-1
        while (i:=i+1) < len(rem):
            if ')' in rem[i]:
                indexOf[')'] = i
                sc_value = str(bafs_eval(rem[indexOf['(']+1:indexOf[')']], False))+rem[indexOf[')']][1:]
                sc_value, jump = fix_bracket(sc_value, rem[indexOf[')']+1:])
                sc_value = start[:-1]+sc_value
                return sc_value, jump+i
                
        return bafs_eval([start]+rem), 'finish'

    i=-1
    while (i:=i+1) < len(res):
        e = res[i]
        if indexOf['('] != -1:
            if ')' in e:
                indexOf[')'] = i
                sc_value, jump = fix_bracket(str(bafs_eval(res[indexOf['(']+1:indexOf[')']], False))+res[indexOf[')']][1:], res[indexOf[')']+1:])
                if jump == 'finish':
                    break
                i+=jump
                _res += [res[indexOf['(']][:-1]+sc_value]
                indexOf['('] = indexOf[')'] = -1
            continue
        if '(' in e:
            indexOf['('] = i
        else:
            _res += [e]
    res = _res
    for e in res:
        if e == '':continue
        if (v:=tryParseFloat(e)) != None or (v:=tryEval(e)) != None:
            value += v
            continue
        _e = ' ' + e + ' '
        for o in OPER['SIMPLE']:
            _e = _e.replace(o, ' '+o+' ')
        _e = var_2_val(_e.split(' '))
        se = ''
        for __e in _e:
            se += str(__e)
        if (v:=tryEval(se)) != None:
            value += v
            continue
        if e in _(*OPERS):
            if value - int(value) == 0:
                value = int(value)
            values += [value]
            Print('grp', len(values), ':', value)
            value = 0
            opers += [e]
    if value - int(value) == 0:
        value = int(value)
    values += [value]
    Print('grp', len(values), ':', value)
    equation = ''
    for i in range(len(opers)):
        if opers[i] in _(*SPECIAL_OPERS):
            if opers[i] in OPER['SPECIA']['ANTI']:
                if i==0 or opers[i-1] == '+':
                    opers[i] = '-'
                else: opers[i] = '+'
        if opers[i] in _(*OPER['SPEC_LOGIC']):
            opers[i] = OPER['SPEC_LOGIC'][opers[i].upper()]
        equation += str(values[i]) + opers[i]
    equation += str(values[-1])
    Print('equation:',equation)
    final_value = eval(equation)
    variables['ans'] = final_value
    return final_value

def localParm(param):
    return '$\n$' + str(param) + '$'

code = ""
recording = False
def compile(r, printOut=True):
    global recording, code
    if recording:
        if r in _('run', 'end'):
            recording = False
            compile(code)
            print('Ran')
            return
        code += r + '\\n'
        return
    codes = r.replace(')(', ')*(').replace('(', '( ')\
        .replace(')',' )').replace('^', '**')\
        .split(' ')
    match codes[0]:
        case x if x in _('start','record'):
            print("Started")
            recording = True
            code = ""
        case 'var'|'$':
            if codes[1] == '=':
                variables[codes[1]] = v = bafs_eval(codes[3:])
            else:
                variables[codes[1]] = v = bafs_eval(codes[2:])
            print('var', codes[1], '=', v)
        case 'def':
            try:
                exec(r.replace('\\n', '\n'))
                def getFunc(func):
                    indexOf = {'(':-1, ')':-1}
                    for i in range(len(func)):
                        if indexOf['(']==-1 and func[i] == '(':
                            indexOf['('] = i
                        if func[i] == ')':
                            indexOf[')'] = i
                    return func[4:indexOf['(']].replace(' ', ''), func[indexOf['(']+1:indexOf[')']]
                funcName, params = getFunc(r)
                exec("""
funcs['%(n)s'] = {
    'function': %(n)s,
    'params': [%(p)s]
}
""" % {'n':funcName, 'p':"'"+params.replace(',', "','")+"'"})
                print(r.replace('\\n', '\n'))
                return
            except Exception as e:
                log(e)
                pass
            params = []
            dig_to = 0
            for i in range(2, len(codes)):
                if codes[i] in ['to', ':', '=>']:
                    dig_to = i
                    break
                else:
                    params += [codes[i]]
            if not dig_to:
                params = []
                for i in range(2, len(codes)):
                    inParams = False
                    for p in params:
                        if codes[i][:len(p)] == p:
                            inParams = True
                            break
                    if inParams or codes[i][0] in "0123456789(":
                        codes = codes[:i] + [':'] + codes[i:]
                        dig_to = i
                        break
                    else:
                        params += [codes[i]]
                string = ''
                for i in range(dig_to+1, len(codes)):
                    string += codes[i] + ' '
            original_string = string
            for p in params:
                string = string.replace(p, localParm(p))
            funcs[codes[1]] = {
                'equation':string,
                'params':params,
            }
            def param_fix(string):
                return str(string).replace('[','(').replace(']',')').replace("'",'')
            print('func', codes[1], ':', param_fix(params), '=>', original_string.replace('( ','(').replace(' )', ')'))
            return
        case 'func':
            runFunc(codes,1)
        case x if x in funcs:
            runFunc(codes,0)
        case _:
            printOut and print('Final Answer:',bafs_eval(codes))

while (r := input('input: ')) != 'stop': compile(r)