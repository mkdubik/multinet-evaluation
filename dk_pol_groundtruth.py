import csv 
import collections

from pprint import pprint
from pdb import set_trace

upp_name = []

pol = collections.defaultdict(list)

with open('/home/guest/Downloads/updated_list_id.csv', 'r') as c:
	next(c)
	d = csv.reader(c, delimiter = ';')


	for row in d:
		pol[row[2]].append(row[-2])
		upp_name.append(row[2])


upp_name = set(upp_name)


for k, v in pol.items():
	print k, v

set_trace()	

clustered = []
with open('/home/guest/multinet/multinet/test/DK_Pol_lart.txt', 'r') as c:
	for row in d:
		pass
print pol
#print upp
