import sys
from pdb import set_trace

data = {}
with open(sys.argv[1]) as fd:
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
	fl = 'pass_{p}_{dataset}_{move}_{gamma}_{omega}.log'
	memory = {}

	for p in obj['passes'].keys():
		tmp = fl.format(p = p,
						dataset = obj['params']['dataset'],
						move = obj['params']['move'],
						gamma = obj['params']['gamma'],
						omega = obj['params']['omega'])

		with open('telemetry/glouvain/' + tmp) as fd:
			memory[p] = process_memory_log(fd.read())

	obj['memory'] = memory

def process_pmm(obj):
	svd = 'svd_{dataset}_{k}_{ell}.log'
	kmeans = 'kmeans_{dataset}_{k}_{ell}.log'
	memory = {}

	if 'error' in obj['params']:
		return

	tmp = svd.format(dataset = obj['params']['dataset'],
					k = obj['params']['k'],
					ell = obj['params']['ell'])

	with open('telemetry/pmm/' + tmp) as fd:
		memory['svd'] = process_memory_log(fd.read())

	tmp = kmeans.format(dataset = obj['params']['dataset'],
					k = obj['params']['k'],
					ell = obj['params']['ell'])

	with open('telemetry/pmm/' + tmp) as fd:
		memory['kmeans'] = process_memory_log(fd.read())

	obj['memory'] = memory




methods = {
	"lart": process_lart,
	"glouvain": process_glouvain,
	"pmm": process_pmm
}

for obj in data:
	func = methods[obj['method']]
	func(obj)


set_trace()
