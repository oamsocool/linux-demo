import re,sys,os

def get_module_name(*args):
    filename = args[0]
    with open(filename, 'r+') as f:
        for line in f:
            if line.strip().startswith('module_name'):
                return line.strip().split(':')[1]
        raise Exception("Not Match [module_name].")

def set_module_name(*args):
    filename = args[0]
    name = args[1]
    f = open(filename, 'w+')
    f.writelines(['module_name:{}\n'.format(name)])
    f.close()

def get_workspace_name(*args):
    filename = args[0]
    with open(filename, 'r+') as f:
        for line in f:
            if line.strip().startswith('workspace'):
                return line.strip().split(':')[1]
        raise Exception("Not Match [workspace].")

def set_workspace_name(*args):
    filename = args[0]
    name = args[1]
    f = open(filename, 'a')
    f.writelines(['workspace:{}\n'.format(name)])
    f.close()


if __name__ == "__main__":
    actions = {'get_module_name' :get_module_name,
               'set_module_name' :set_module_name,
               'get_workspace_name':get_workspace_name,
               'set_workspace_name':set_workspace_name,}

    print(actions[sys.argv[1]](*sys.argv[2:]))

