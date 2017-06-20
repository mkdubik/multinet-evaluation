import sys
from pdb import set_trace
import json

class aggregate_results:

	def aggregate(self, name):
		data = {}
		with open(name) as fd:
			for line in fd.readlines():
				method, dataset, runtime = line.split(',')

				try:
					with open('%s_%s.log' % (method, dataset)) as fm:
						data[method] = [runtime, process_memory_log(fm.read())]
				except:
					pass

		return data

	def process_memory_log(self, contents):
		mem = [item for item in contents.split('\n')]
		for m in mem:
			k, v = m.split(':')
			if k == 'VmRSS':
				return v.replace('\t', '').split()[0]

		raise Exception(contents + " missing VmRSS")


def main():
	from json import dumps
	from pdb import set_trace;
	a = aggregate_results()
	data = a.aggregate(sys.argv[1])

	set_trace()

main()








