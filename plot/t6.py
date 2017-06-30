from pdb import set_trace
from os import listdir
import matplotlib.pyplot as plt
import operator
import collections
from itertools import count

import networkx as nx
import numpy as np

from t5 import collect_data



def collect_community(path):
	files = filter(lambda x: '_community' in x, listdir(path))
	data = {
		'lart':{}, 'glouvain':{}, 'pmm':{},
	}

	for file in files:
		params = file.split('_')
		method = params[0]
		if method == 'pmm' or method == 'glouvain':
			params = [params[1], params[2]]
		if method == 'lart':
			params = [params[1]]

		with open(path + file) as fd:
			d = fd.read()

		data[method]['_'.join(['%.1f' % (float(p)) for p in params])] = d

	return data



def main():
	data = collect_data('results/t6/')
	comm = collect_community('results/t6/')

	def find_best(data, method):
		key = max(data[method].iteritems(), key=operator.itemgetter(1))[0]
		y = data[method][key]
		return y, key, method

	print data['glouvain']['1.0_1.0']
	print data['lart']['1.0']
	print data['pmm']['30.0_140.0']


	gl = find_best(data, 'glouvain')
	ll = find_best(data, 'lart')
	pl = find_best(data, 'pmm')

	print gl
	print ll
	print pl

	best = max([gl, ll, pl], key=operator.itemgetter(0))
	best_comm = {}
	for b in comm[best[2]][best[1]].split('\n'):
		if b:
			a,l,c = b.split(',')
			best_comm['%s-%s' % (a, l)] = int(c)

	layers = {
		'RT': nx.Graph(), 'ff': nx.Graph(), 'Re': nx.Graph()
	}
	ids = {}

	counter = 0
	groups = []
	with open('data/t6/dk', 'r') as fd:
		for l in fd.readlines():
			a1,a2, layer = l.replace('\n', '').split(",")

			if a1 not in ids:
				ids[a1] = counter
				counter = counter + 1

			if a2 not in ids:
				ids[a2] = counter
				counter = counter + 1

			groups.append(best_comm['%s-%s' % (a1, layer)])
			groups.append(best_comm['%s-%s' % (a2, layer)])

			for k,v in layers.iteritems():
				v.add_node(ids[a1], label = best_comm['%s-%s' % (a1, layer)])
				v.add_node(ids[a2], label = best_comm['%s-%s' % (a2, layer)])

			layers[layer].add_edge(ids[a1], ids[a2])

	truth = {}
	with open('data/t6/dk_truth', 'r') as fd:
		for l in fd.readlines():
			actor, party = l.replace('\n', '').split(',')
			truth[actor] = party

	mapping = dict(zip(sorted(groups), count()))

	N, L = len(layers['ff'].nodes()), len(layers.keys())
	adj_block = mx.lil_matrix(np.zeros((N * L, N * L)))

	for i in xrange(L):
		for j in xrange(L):
			if i < j:
				adj_block[N * i: N * (i + 1), N * j: (j+1) * N] = np.identity(N)

	adj_block += adj_block.T

	mg = mx.MultilayerGraph(list_of_layers=[v for k, v in layers.items()])
		#inter_adjacency_matrix = adj_block)

	mg.set_edges_weights(intra_layer_edges_weight=1)
	#                     inter_layer_edges_weight=2)


	fig = plt.figure(figsize=(10, 10))
	plt.title('Twitter data of the Danish 2015 election')

	stats = collections.defaultdict(list)

	for k, v in best_comm.items():
		stats[v].append(k)

	stats2 = collections.defaultdict(dict)
	for k,v in stats.items():
		for e in v:
			actor,_ = e.split('-')

			if truth[actor] in stats2[k]:
				stats2[k][truth[actor]] += 1
			else:
				stats2[k][truth[actor]] = 1

	left = [
		'Dansk Folkeparti',
		'Venstre',
		'Liberal Alliance',
		'Det Konservative Folkeparti',
		'KristenDemokraterne',
	]

	right = [
		'Socialistisk Folkeparti',
		'Radikale Venstre',
		'Socialdemokratiet',
		'Alternativet',
		'Enhedslisten'
	]
	out = 'Udenfor partierne'

	for k,v in stats2.items():
		total = 0
		for k1,v1 in v.items():
			total += v1

		pscore = 0
		for k1,v1 in v.items():
			if k1 in left:
				pscore += (stats2[k][k1] * 1)
			if k1 in right:
				pscore -= (stats2[k][k1] * 1)

			stats2[k][k1] = float(v1) / float(total)

		stats2[k]['pscore'] = pscore

	stats2 = dict(stats2)
	colors = [stats2[mg.node[n]['label']]['pscore'] for n in mg.nodes()]


	from pprint import pprint
	pprint(stats2)


	pos = mx.get_position(mg,
						nx.spring_layout(layers[layers.keys()[2]], weight = 'pscore'),
						#mx.fruchterman_reingold_layout(layers[layers.keys()[2]], dim = 3, k = 1, iterations = 1000),
						layer_vertical_shift=0.2,
						layer_horizontal_shift=0.0,
						proj_angle=47)
	mx.draw_networkx(mg,pos=pos,node_size=100, with_labels=False,
					labels = {k:v['label'] if 'label' in v else 'missing' for k, v in mg.node.items()},
					node_color = colors,
					cmap=plt.get_cmap('RdBu_r'),
					edge_color=[mg[a][b]['weight'] for a,b in mg.edges()])

	import matplotlib.patches as mpatches
	red_patch = mpatches.Patch(color='red', label='The red data')

	#plt.legend(handles=[red_patch])
	#ax2.legend(scatterpoints=1)
	plt.xticks([])
	plt.yticks([])
	plt.show()


if __name__ == "__main__":
	from os import sys, path
	sys.path.append(path.dirname(path.dirname(path.abspath(__file__))) + '/plot')
	print sys.path
	import multinetx as mx
	main()
