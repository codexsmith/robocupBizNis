from decimal import Decimal

#basis formation
infile = 'defense-formation.conf'
#new formation
outfile = 'positive-formation.conf'

with open(infile) as inf, open(outfile, "w") as outf:
	for num, line in enumerate(inf, 1):
		split = line.split()
		out = ''
		if num < 16 or split[0] == '-----' or split[0] == 'Ball' or split[0] == '1' or split[0] == 'End':
			out = line
		else:
			# Modify this line. split[1] is X and split[2] is Y. Middle of field is (0, 0)
			out = split[0] + ' ' + str(Decimal(split[1]) / 2 + 25) + ' ' + split[2] + '\n'
		outf.writelines(out)
