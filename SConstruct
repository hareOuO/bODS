import os

env=Environment(ENV=os.environ, tools=['default'])

print(env['ENV']['HOME'])
print(env['ENV']['PATH'])

if 'CC' in os.environ:
    env['CC']=os.environ['CC']
    
if 'CXX' in os.environ:
    env['CXX']=os.environ['CXX']

root_dir = Dir('#').srcnode().abspath

config = {}




if 'config_file' in ARGUMENTS:
    SConscript(ARGUMENTS['config_file'], exports=['env','config'])



env.Append(CCFLAGS = ['-fPIC','-Wall', '-march=native'])
env.Append(CXXFLAGS = ['-std=c++14'])
env.Append(CPPPATH = ['/usr/local/include'])
env.Append(LIBPATH = ['/usr/local/lib'])

env.Append(LIBS = ['crypto', 'pthread', 'dl', 'snappy', 'z', 'bz2',  'lz4'])

#Workaround for OS X
if env['PLATFORM'] == 'darwin':
    rpathprefix = '-rpath'
    env.Append(LINKFLAGS = [[rpathprefix, lib] for lib in env['RPATH']])
    env.Append(CPPPATH=['/usr/local/opt/openssl/include'])
    env.Append(LIBPATH=['/usr/local/opt/openssl/lib'])    
    # env.Append(LINKFLAGS = ['-rpath', cryto_lib_dir+'/lib'])



env['STATIC_AND_SHARED_OBJECTS_ARE_THE_SAME']=1


debug = ARGUMENTS.get('debug', 0)
if int(debug):
	env.Append(CCFLAGS = ['-O3'])
else:
	env.Append(CCFLAGS = ['-g', '-O0'])

static_relic = ARGUMENTS.get('static_relic', 0)

env.Append(CPPDEFINES = ['BENCHMARK'])

def run_test(target, source, env):
    app = str(source[0].abspath)
    if os.spawnl(os.P_WAIT, app, app)==0:
        return 0
    else:
        return 1

bld = Builder(action = run_test)
env.Append(BUILDERS = {'Test' :  bld})


env.Alias('deps', [])

objects = SConscript('src/build.scons', exports='env', variant_dir='build')
env.Depends(objects["omap"],[])


Clean(objects["omap"], 'build')

outter_env = env.Clone()
outter_env.Append(CPPPATH = ['build'])

client_debug_prog   = outter_env.Program('test_omap',    ['test_omap.cpp']     + objects["omap"])
server_debug_prog   = outter_env.Program('test_server',  ['test_server.cpp']  + objects["omap"])

env.Alias('omap', [client_debug_prog])
env.Alias('omap', [server_debug_prog])

env.Default(['omap'])
