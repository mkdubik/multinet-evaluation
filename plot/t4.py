from os import listdir
import matplotlib.pyplot as plt

from pdb import set_trace

def collect_data(path):
	data = {
		1000: {'lart':{}, 'glouvain':{}, 'pmm':{}},
		4000: {'lart':{}, 'glouvain':{}, 'pmm':{}},
	}
	files = listdir(path)

	for file in files:
		method, actors, layers, mu = file.split('_')

		nmi = 0
		with open(path + '/' + file) as fd:
			_, _, nmi, _ = fd.read().split(',')


		data[int(actors)][method][float(mu)] = float(nmi)

	return data


def main():
	data = collect_data('results/t4/')
	x = sorted([key / 10 for key in data[data.keys()[0]]['glouvain'].keys()])

	def plot(data, method, actors):
		y = data[actors][method]
		y = [y[k] for k in sorted(y.keys())]
		a1, = plt.plot(x, y, '-', linewidth=2, label = method)
		return a1
	plt.figure(1)

	a = plot(data, 'glouvain', 1000)
	b = plot(data, 'lart', 1000)
	c = plot(data, 'pmm', 1000)

	plt.xlabel('Mixing parameter, $\mu$', fontsize = 14)
	plt.ylabel('Normalized Mutual Information (NMI)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 3, title = 'N = 1000, L = 3')
	plt.xticks(x)
	plt.yticks([float(x) / 10 for x in xrange(11)])
	plt.savefig('t4a')

	'''
	a = plot(data, 'glouvain', 4000)
	b = plot(data, 'lart', 4000)
	c = plot(data, 'pmm', 4000)

	plt.xlabel('Mixing parameter, $\mu$', fontsize = 14)
	plt.ylabel('Normalized Mutual Information (NMI)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 3, title = 'N = 4000, L = 3')
	plt.xticks(x)
	plt.yticks([float(x) / 10 for x in xrange(11)])
	plt.savefig('t4a')
	'''



if __name__ == "__main__":
	main()
