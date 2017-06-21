from pdb import set_trace
import multinetx as mx
import networkx as nx
import numpy as np
import sys

import matplotlib.pyplot as plt
import collections

_data = sys.argv[1]
_community = sys.argv[2]

data = set()

edges = collections.defaultdict(lambda: collections.defaultdict(list))

with open(_data, 'r') as fd:
	for l in fd.readlines():
		a1,a2, layer = l.replace('\n', '').split(" ")
		data.update([a1, a2])

		if a1 == a2:
			continue

		edges[layer][a1].append(a2)

data = list(data)

truth = collections.defaultdict(lambda: collections.defaultdict(dict))#{}


ccid = []
with open(_community, 'r') as fd:
	for l in fd.readlines():
		a, layer, cid = l.replace('\n', '').split(",")
		truth[a][layer] = cid
		ccid.append(int(cid))

layers = {v : nx.Graph() for v in edges.keys()}

val_map = {
	0: 'r',
	1: 'g',
	2: 'b',
	3: 'c',
	4: 'm',
	5: 'y',
	6: 'k',
	7: 'w',
	8: 'b',
	9: 'p',
}

node_color=range(max(ccid) + 1)
values = []

for i in xrange(len(data)):
	tmp = []
	for k in layers.keys():
		try:
			group = int(truth[data[i]][k])
		except:
			group = 8
		layers[k].add_node(i, label = data[i], group = group)
		tmp.append(val_map.get(group, 'g'))
	values += tmp

for i in edges.keys():
	for j in edges[i].keys():
		for k in edges[i][j]:
			layers[i].add_edge(data.index(j), data.index(k), weight=1, color = 'g')
			layers[i].add_edge(data.index(k), data.index(j), weight=1, color = 'g')

N, L = len(data), len(layers.keys())
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


fig = plt.figure()

ax2 = fig.add_subplot(111)
ax2.axis('off')
ax2.set_title('DK Politics')
pos = mx.get_position(mg,mx.fruchterman_reingold_layout(layers[layers.keys()[2]]),
                      layer_vertical_shift=0.2,
                      layer_horizontal_shift=0.0,
                      proj_angle=47)

mx.draw_networkx(mg,pos=pos,node_size=100, with_labels=False,
				labels = {k:v['label'] if 'label' in v else 'missing' for k, v in mg.node.items()},
				node_color = [node_color[v['group']] if 'group' in v else 0 for k, v in mg.node.items()],
				node_cmap=plt.cm.jet_r,
				edge_color=[mg[a][b]['weight'] for a,b in mg.edges()])

plt.show()


