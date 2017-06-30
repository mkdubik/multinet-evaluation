#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from os import listdir
from pdb import set_trace
import matplotlib.pyplot as plt

def process_memory_log(contents):
	mem = [item for item in contents.split('\n')]
	for m in mem:
		k, v = m.split(':')
		if k == 'VmHWM':
			return v.replace('\t', '').split()[0]

	raise Exception(contents + " missing VmHWM")

def collect_data(path):
	data = {'lart':{}, 'glouvain':{}, 'pmm':{}}
	files = filter(lambda x : '.log' not in x , listdir(path))

	for file in files:
		method, actors, layers = file.split('_')
		running_time = 0
		with open(path + '/' + file) as fd:
			running_time = float(fd.read()) / 1000

		with open(path + '/' + file + '.log') as fd:
			memory = float(process_memory_log(fd.read())) / 1000

		data[method][int(actors)] = [running_time, memory]

	return data

def main():
	plt.figure(1)

	data = collect_data('telemetry/t1/')
	x =  sorted(data[data.keys()[0]].keys())


	def plot(data, method, what):
		y = data[method]
		y = [y[k][what] for k in sorted(y.keys())]
		a1, = plt.plot(x, y, '-', linewidth=2, label = method)
		return a1

	a = plot(data, 'glouvain', 0)
	b = plot(data, 'pmm', 0)
	c = plot(data, 'lart', 0)

	plt.xlabel('Actors (thousand)', fontsize = 14)
	plt.ylabel('computational time (ms)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 2, title = 'L = 3, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t1a')


	plt.figure(2)

	a = plot(data, 'glouvain', 1)
	b = plot(data, 'pmm', 1)
	c = plot(data, 'lart', 1)

	plt.xlabel('Actors (thousand)', fontsize = 14)
	plt.ylabel('peak memory usage (megabytes)', fontsize = 14)
	plt.legend(handles = [a, b, c], loc = 2, title = 'L = 3, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t1b')

if __name__ == "__main__":
	main()
