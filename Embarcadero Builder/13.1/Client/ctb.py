for i in range(256):
    print('case {0}:\n\treturn "{1}";\n'.format(i,'0'*(2-len(hex(i)[2:]))+hex(i)[2:].upper()))
