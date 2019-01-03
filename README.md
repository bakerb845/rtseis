# Background

RTSeis is a real-time focused seismic signals processing library that is intended to

   1. Facilitate real-time seismic signals processing in a production environment.
   2. Increase the productivity of scientists at regional seismic networks by providing convenient C++ and Python solutions with minimal sacrifice in performance.
   3. Emphasize data parallelism to achieve high-performance for processing of Large-N datasets.

## History

This approximately the third iteration of this library and is offshoot of my time at [ISTI](http://www.isti.com/) where I'd frequently have to write holistic applications that do some generic signals processing and, with that processed data, something project specific.  The generic part is the focus of this library where the goal is to help network seismologists easily implement a new processing workflow.

## Alternatives

There are a great many signals processing packages.  In general, RTSeis tries to distinguish itself by 
 
   1. Being accessible from C.
   2. Focusing on the real-time component of signals processing.

These points are really driven by the target audience which is seismologists in network operations.  Indeed, these focuses may even make RTSeis a suboptimal solution as the first point requires compilation and clearing library dependencies while the second point can diminish some algorithm performance.

### SciPy

RTSeis's focus is basic filtering of seismic data.  It is most analogous to SciPy's signal's processing package.  The main difference is that RTSeis has more support for real-time signals processing.

### ObsPy

ObsPy is a feature-rich platform for seismic processing with an easy-to-use Python interface.  As ObsPy uses SciPy to implement many of its signals processing routines it is, at the time of writing, limited in its real-time applications.  Additionally, RTSeis's Apache 2 license does not involve a copyleft clause which may be advantageous for those looking to create closed-source solutions.

### Matlab

Matlab is a complete signals processing solution.  RTSeis cannot even begin to approximate Matlab's functionality.  However, what RTSeis can do is provide a solution that doesn't require developers to have a Matlab license.

# Acknowledgements

Much of this development was financed by ISTI.  However, the real heroes are beta testers who put this library through its paces.

