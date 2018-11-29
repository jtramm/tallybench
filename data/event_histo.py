import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt

dat = np.loadtxt('event_stats.txt')

mu = np.mean(dat)
sigma = np.std(dat)
print("mu = ", mu, "sigma = ", sigma)

vals = np.arange(0, np.max(dat))

f = plt.figure()
entries, bin_edges, patches = plt.hist(dat, bins = vals, normed=1)
# calculate binmiddles
plt.xlabel("Events per Particle History")
plt.ylabel("Probability")
plt.title("PDF of Number of Events per Particle History\n $\mathrm{\mu}$ = %.2f $\mathrm{\sigma}$ = %.2f" %(mu, sigma))

y = mlab.normpdf( bin_edges, mu, sigma)
l = plt.plot(bin_edges, y, 'r--', linewidth=1)

#plt.grid(True)

plt.show()

#f.savefig("event_PDF.pdf", bbox_inches='tight')
