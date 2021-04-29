import os
import csv
import json

if __name__ == '__main__':
	start = 0
	end = 1
	for n in range(start, end):
		fname = f"real.{n+1}"
		data = {'caches' : [] }
		cache_heading = { 'reads' : [], 'read misses' : [], 'writes' : [], 'write misses' : [],
							'miss rate' : [], 'writebacks' : [], 'c2ctx' : [],
							'interventions' : [], 'invalidations' : [], 'sigrds' : [],
							'sigrdxs' : [], 'sigupgrs' : [] }
		with open(fname, 'r') as rfile:
			lines = rfile.readlines()
			num_caches = 0
			for line in lines:
				if 'COHERENCE PROTOCOL' in line:
					data['protocol'] = line.split(':')[1].translate(str.maketrans('','',' \n\t\r'))
				elif 'L1_SIZE' in line:
					data['size'] = line.split(':')[1].translate(str.maketrans('','',' \n\t\r'))
				elif 'L1_ASSOC' in line:
					data['assoc'] = line.split(':')[1].translate(str.maketrans('','',' \n\t\r'))
				elif 'L1_BLOCKSIZE' in line:
					data['blksize'] = line.split(':')[1].translate(str.maketrans('','',' \n\t\r'))
				elif 'number of reads' in line:
					data['caches'].append({})
					data['caches'][num_caches]['reads'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of read misses' in line:
					data['caches'][num_caches]['read misses'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of writes' in line:
					data['caches'][num_caches]['writes'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of write misses' in line:
					data['caches'][num_caches]['write misses'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'total miss rate' in line:
					data['caches'][num_caches]['miss rate'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of writebacks' in line:
					data['caches'][num_caches]['writebacks'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of cache-to-cache' in line:
					data['caches'][num_caches]['c2ctx'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of SignalRds' in line:
					data['caches'][num_caches]['sigrds'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of SignalRdXs' in line:
					data['caches'][num_caches]['sigrdxs'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of SignalUpgrs' in line:
					data['caches'][num_caches]['sigupgrs'] = (line.split('s')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of invalidations' in line:
					data['caches'][num_caches]['invalidations'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
				elif 'number of interventions' in line:
					data['caches'][num_caches]['interventions'] = (line.split(':')[1].translate(str.maketrans('','',' \n\t\r')))
					num_caches = num_caches + 1
		# dump data to json file
		outfile_name = f"{data['protocol']}_blk{data['blksize']}_size{data['size']}_assoc{data['assoc']}"
		with open(f"{outfile_name}.json", 'w') as outfile:
			json.dump(data, outfile)
		# write the csv files
		with open(f"{outfile_name}.csv", 'w', newline='', encoding='utf-8') as outfile:
			writer = csv.DictWriter(outfile, fieldnames=cache_heading.keys())
			writer.writeheader()
			writer.writerows(data['caches'])
