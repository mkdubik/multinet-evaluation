from os import listdir
import matplotlib.pyplot as plt
import operator
from pdb import set_trace

def collect_data(path):
	data = {
		'lart':{}, 'glouvain':{}, 'pmm':{},
	}
	files = filter(lambda x: '_community' not in x, listdir(path))

	for file in files:
		params = file.split('_')
		method = params[0]
		if method == 'pmm' or method == 'glouvain':
			params = [params[1], params[2]]
		if method == 'lart':
			params = [params[1]]

		nmi = 0
		with open(path + '/' + file) as fd:
			_, _, nmi, _ = fd.read().split(',')

		data[method]['_'.join(['%.1f' % (float(p)) for p in params])] = float(nmi)

	return data


def main():
	data = collect_data('results/t5/')

	def best(data, method):
		key = max(data[method].iteritems(), key=operator.itemgetter(1))[0]
		y = data[method][key]
		return y, key

	print data['glouvain']['1.0_1.0']
	print data['lart']['1.0']
	print data['pmm']['30.0_140.0']

	print best(data, 'glouvain')
	print best(data, 'lart')
	print best(data, 'pmm')


if __name__ == "__main__":
	main()
