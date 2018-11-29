import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt

dat = np.loadtxt('tally_stats.txt')

mu = np.mean(dat)
sigma = np.std(dat)
print("mu = ", mu, "sigma = ", sigma)

vals = np.arange(np.max(dat))
print(vals)

f = plt.figure()
n, bins, patches = plt.hist(dat, bins = vals, normed=1)
#bins = 75
plt.xlabel("Tallies per Particle History")
plt.ylabel("Probability")
#plt.title("PDF of Number of events per particle history.\n mean = %.2f sigma = %.2f" %(mu, sigma))
plt.title("PDF of Number of Tallies per Particle History\n $\mathrm{\mu}$ = %.2f $\mathrm{\sigma}$ = %.2f" %(mu, sigma))

y = mlab.normpdf( bins, mu, sigma)
l = plt.plot(bins, y, 'r--', linewidth=1)

#plt.grid(True)

#plt.show()

f.savefig("tally_PDF.pdf", bbox_inches='tight')
