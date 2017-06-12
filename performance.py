import operator
from pdb import set_trace
import json

glouvain = json.loads(eval(open('glouvain_1-5k.json').read()))
lart = json.loads(eval(open('1-5kLARt.json').read()))
pmm = json.loads(eval(open('1-5kpmm.json').read()))


def bench(name, size, dataset):
	data = filter(lambda x: x['params']['dataset'] == '%s_mix01' % size, dataset) + filter(lambda x: x['params']['dataset'] == '%s_mix025'  % size, dataset) + filter(lambda x: x['params']['dataset'] == '%s_mix04' % size, dataset)

	avg = 0
	mem = 0
	nmi = 0
	for i, p in enumerate(data):
		mem += int(max(p['memory'].iteritems(), key=operator.itemgetter(1))[1])

		tmp = p['accuracy']['nmi']
		if tmp > nmi:
			nmi = tmp

		if 'total' in p:
			avg += p['total']
			continue
		if 'time' in p:
			avg += p['time']
			continue
		if 'timestamps' in p:
			avg += p['timestamps']['total']
			continue
		if 'passes' in p:
			avg += p['passes']['time']


	print '%s average time for %s: ' % (name, size) , avg / float(i) / 1000, 'seconds'
	print '%s average memory for %s: ' % (name, size) , (mem / float(i) / 1000) / 1000, 'Gb'


bench('pmm', '1k', pmm)
bench('glouvain', '1k', glouvain)
bench('lart', '1k', lart)

bench('pmm', '3k', pmm)
bench('glouvain', '3k', glouvain)
bench('lart', '3k', lart)

bench('pmm', '5k', pmm)
bench('glouvain', '5k', glouvain)
bench('lart', '5k', lart)




def nmi(name, size, dataset):
	def best(data):
		nmi, modularity = 0, 0
		for d in data:
			tmp = float(d['accuracy']['nmi'])
			if tmp > nmi:
				nmi = tmp
				modularity = float(d['accuracy']['modularity_result'])

		return nmi, modularity

	data_01 = filter(lambda x: x['params']['dataset'] == '%s_mix01' % size, dataset)
	data_025 = filter(lambda x: x['params']['dataset'] == '%s_mix025'  % size, dataset)
	data_04  = filter(lambda x: x['params']['dataset'] == '%s_mix04' % size, dataset)

	nmi, modularity = best(data_01)
	print 'Best nmi for %s %s_01' % (name, size), nmi, 'modularity', modularity
	nmi, modularity = best(data_025)
	print 'Best nmi for %s %s_025' % (name, size), nmi, 'modularity', modularity
	nmi, modularity = best(data_04)
	print 'Best nmi for %s %s_04' % (name, size), nmi, 'modularity', modularity


nmi('pmm', '1k', pmm)
nmi('glouvain', '1k', glouvain)
nmi('lart', '1k', lart)

nmi('pmm', '3k', pmm)
nmi('glouvain', '3k', glouvain)
nmi('lart', '3k', lart)

nmi('pmm', '5k', pmm)
nmi('glouvain', '5k', glouvain)
nmi('lart', '5k', lart)

