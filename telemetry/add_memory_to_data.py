from pdb import set_trace

data = {}
with open('telemetry/raw.json') as fd:
	data = eval(fd.read().replace(',\n]', ']').replace('},\n]', '}]\n'))

def process_memory_log(contents):
	mem = [item for item in contents.split('\n')]
	for m in mem:
		k, v = m.split(':')
		if k == 'VmRSS':
			return v.replace('\t', '').split()[0]

	raise Exception(contents + " missing VmRSS")

def process_lart(obj):
	fl = '{function}_{dataset}_{walks}_{eps}_{gamma}.log'
	memory = {}

	for func in obj['timestamps'].keys():
		if func == 'total':
			func = 'end'

		tmp = fl.format(function = func,
						dataset = obj['params']['dataset'],
						walks = obj['params']['walks'],
						eps = obj['params']['eps'],
						gamma = obj['params']['gamma'])

		with open('telemetry/lart/' + tmp) as fd:
			memory[func] = process_memory_log(fd.read())

	obj['memory'] = memory

def process_glouvain(obj):
	set_trace()
	pass

def process_pmm(obj):
	set_trace()
	pass

methods = {
	"lart": process_lart,
	"glouvain": process_glouvain,
	"pmm": process_pmm
}


for obj in data:
	func = methods[obj['method']]
	func(obj)

