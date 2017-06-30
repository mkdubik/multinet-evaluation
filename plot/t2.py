from t1 import process_memory_log
from os import listdir
import matplotlib.pyplot as plt

from pdb import set_trace

def collect_data(path):
	data = {
		1000: {'lart':{}, 'glouvain':{}, 'pmm':{}},
		4000: {'lart':{}, 'glouvain':{}, 'pmm':{}},
	}
	files = filter(lambda x : '.log' not in x , listdir(path))

	for file in files:
		method, actors, layers = file.split('_')
		running_time = 0

		try:
			with open(path + '/' + file) as fd:
				running_time = float(fd.read()) / 1000
		except:
			running_time = float("nan")

		try:
			with open(path + '/' + file + '.log') as fd:
				memory = float(process_memory_log(fd.read())) / 1000
		except:
			memory = float("nan")

		data[int(actors)][method][int(layers)] = [running_time, memory]

	return data

def main():
	data = collect_data('telemetry/t2/')
	x = sorted(data[data.keys()[0]]['glouvain'].keys())

	def plot(data, method, what, actors):
		y = data[actors][method]
		y = [y[k][what] for k in sorted(y.keys())]
		a1, = plt.plot(x, y, '-', linewidth=2, label = method)
		return a1

	if not 5 in data[4000]['lart']:
		data[4000]['lart'][5] = [float('nan'), float('nan')]

	plt.figure(1)

	a = plot(data, 'glouvain', 0, 1000)
	b = plot(data, 'pmm', 0, 1000)
	c = plot(data, 'lart', 0, 1000)

	plt.xlabel('Layers', fontsize = 14)
	plt.ylabel('computational time (ms)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 2, title = 'N = 1000, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t2a')

	plt.figure(2)
	a = plot(data, 'glouvain', 1, 1000)
	b = plot(data, 'pmm', 1, 1000)
	c = plot(data, 'lart', 1, 1000)

	plt.xlabel('Layers', fontsize = 14)
	plt.ylabel('memory usage (megabytes)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 2, title = 'N = 1000, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t2b')

	plt.figure(3)
	a = plot(data, 'glouvain', 0, 4000)
	b = plot(data, 'pmm', 0, 4000)
	c = plot(data, 'lart', 0, 4000)

	plt.xlabel('Layers', fontsize = 14)
	plt.ylabel('computational time (ms)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 2, title = 'N = 4000, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t2c')

	plt.figure(4)
	a = plot(data, 'glouvain', 1, 4000)
	b = plot(data, 'pmm', 1, 4000)
	c = plot(data, 'lart', 1, 4000)

	plt.xlabel('Layers', fontsize = 14)
	plt.ylabel('memory usage (megabytes)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 2, title = 'N = 4000, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t2d')


if __name__ == "__main__":
	main()
