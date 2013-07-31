from decimal import Decimal

#basis formation
infile = 'offense-formation.conf'
#new formation
outfile = 'bias-left-formation.conf'

with open(infile) as inf, open(outfile, "w") as outf:
	for num, line in enumerate(inf, 1):
		split = line.split()
		out = ''
		if num < 16 or split[0] == '-----' or split[0] == 'Ball' or split[0] == '1' or split[0] == 'End':
			out = line
		else:
			# Modify this line. split[1] is X and split[2] is Y. Middle of field is (0, 0)
			out = split[0] + ' ' + split[1] + ' ' + str(Decimal(split[2]) / 2 - 17) + '\n'
		outf.writelines(out)
