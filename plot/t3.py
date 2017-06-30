#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from os import listdir
from pdb import set_trace
import matplotlib.pyplot as plt
from t1 import process_memory_log, collect_data


def main():
	plt.figure(1)

	data = collect_data('telemetry/t3/')
	x =  sorted(data[data.keys()[0]].keys())


	def plot(data, method, what):
		y = data[method]
		y = [y[k][what] for k in sorted(y.keys())]
		a1, = plt.plot(x, y, '-', linewidth=2, label = method)
		return a1

	a = plot(data, 'glouvain', 0)
	c = plot(data, 'pmm', 0)

	plt.xlabel('Actors (thousand)', fontsize = 14)
	plt.ylabel('computational time (ms)', fontsize = 14)
	plt.legend(handles = [a, c], loc = 2, title = 'L = 3, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t3a')


	plt.figure(2)

	a = plot(data, 'glouvain', 1)
	c = plot(data, 'pmm', 1)

	plt.xlabel('Actors (thousand)', fontsize = 14)
	plt.ylabel('peak memory usage (megabytes)', fontsize = 14)
	plt.legend(handles = [a, c], loc = 2, title = 'L = 3, $\mu$ = 0.3')
	plt.xticks(x)
	plt.savefig('t3b')

if __name__ == "__main__":
	main()
