import sys
from pdb import set_trace
import json

class aggregate_results:

	def aggregate(self, name):
		data = {}
		with open(name) as fd:
			data = eval(fd.read().replace(',\n]', ']').replace('},\n]', '}]\n'))

		methods = {
			"lart": self.process_lart,
			"glouvain": self.process_glouvain,
			"pmm": self.process_pmm
		}

		for obj in data:
			func = methods[obj['method']]
			func(obj)

		return data

	def process_memory_log(self, contents):
		mem = [item for item in contents.split('\n')]
		for m in mem:
			k, v = m.split(':')
			if k == 'VmRSS':
				return v.replace('\t', '').split()[0]

		raise Exception(contents + " missing VmRSS")

	def process_accuracy(self, filename):
		accuracy = {}
		with open(filename) as fd:
			modularity_result, modularity_gt, nmi, comm_ratio = fd.read().replace('\n', '').split(',')
			accuracy['modularity_result'] = modularity_result
			accuracy['modularity_gt'] = modularity_gt
			accuracy['nmi'] = nmi
			accuracy['comm_ratio'] = comm_ratio

		return accuracy


	def process_lart(self, obj):
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
				memory[func] = self.process_memory_log(fd.read())

		obj['memory'] = memory
		fl = 'results/{dataset}/LART_{walks}_{eps}_{gamma}'.format(
							dataset = obj['params']['dataset'],
							walks = obj['params']['walks'],
							eps = obj['params']['eps'],
							gamma = obj['params']['gamma'])

		obj['accuracy'] = self.process_accuracy(fl)


	def process_glouvain(self, obj):
		fl = 'pass_{p}_{dataset}_{move}_{gamma}_{omega}.log'
		memory = {}

		for p in obj['passes'].keys():
			tmp = fl.format(p = p,
							dataset = obj['params']['dataset'],
							move = obj['params']['move'],
							gamma = obj['params']['gamma'],
							omega = obj['params']['omega'])


			try:
				with open('telemetry/glouvain/' + tmp) as fd:
					memory[p] = self.process_memory_log(fd.read())
			except:
				pass

		obj['memory'] = memory

		fl = 'results/{dataset}/GLOUVAIN_{gamma}_{omega}_{move}'.format(
							dataset = obj['params']['dataset'],
							gamma = obj['params']['gamma'],
							omega = obj['params']['omega'],
							move = obj['params']['move'])

		obj['accuracy'] = self.process_accuracy(fl)


	def process_pmm(self, obj):
		svd = 'svd_{dataset}_{k}_{ell}.log'
		kmeans = 'kmeans_{dataset}_{k}_{ell}.log'
		memory = {}

		if 'error' in obj['params']:
			return

		tmp = svd.format(dataset = obj['params']['dataset'],
						k = obj['params']['k'],
						ell = obj['params']['ell'])

		with open('telemetry/pmm/' + tmp) as fd:
			memory['svd'] = self.process_memory_log(fd.read())

		tmp = kmeans.format(dataset = obj['params']['dataset'],
						k = obj['params']['k'],
						ell = obj['params']['ell'])

		with open('telemetry/pmm/' + tmp) as fd:
			memory['kmeans'] = self.process_memory_log(fd.read())

		obj['memory'] = memory

		fl = 'results/{dataset}/PMM_{k}_{ell}'.format(
							dataset = obj['params']['dataset'],
							k = obj['params']['k'],
							ell = obj['params']['ell'])

		obj['accuracy'] = self.process_accuracy(fl)



def main():
	from json import dumps
	from pdb import set_trace;
	a = aggregate_results()
	data = a.aggregate(sys.argv[1])

	set_trace()

main()








