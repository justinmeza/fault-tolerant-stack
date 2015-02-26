# Fault Tolerant Stacks

### The Problem

Pointer-based data structures are susceptible to memory faults, which can alter the links between elements.  Such faults can lead to the loss of some or all of the contents of a data structure.  (Imagine a singly-linked list:  A single pointer corrupted by a memory fault makes all the following elements in the list inaccessible.)  Memory faults are easily observable (though often they result in system crashes) and a variety of recent works have characterized the susceptibility of modern DRAM to such faults, which can even be induced by carefully-written programs designed to corrupt data (e.g., [1, 2, 3]).

### Potential Solutions

One way to help reduce the impact of memory faults on pointer-based data structures is to simply replicate the entire contents of the data structure.  This requires a large amount of storage overhead and additional operations to maintain a consistent state among the copies.  In order to survive up to _n_ errors, a data structure must be replicated _n_ + 1 times.

In order to help reduce the overhead required to help tolerate faults, Aumann and Bender [4] designed several fault tolerant data structures.  Their main idea is to use a lower-overhead approach that *permits* up to _n_ elements in a data structure to have faults, so long as the faults remain *isolated* in those _n_ elements.  To do this, they allow elements to be reached via multiple pointer paths.  That way, in the worst case, if _n_ elements becomes unreachable, it does not cause the rest of the data structure to become disconnected and lost.

### This Repository

This repository contains an implementation of a fault tolerant stack (specifically, it implements what Aumann and Bender referred to as a _d_-FTstack, where _d_ is the paramaterized number of faults the stack and tolerate).  It has the same interface as a normal stack: `push`, `pop`, and `head` (to inspect the element on the top of the stack).

See the `main` function in `ftstack.c` for an example of a stack that can tolerate up to 4 faults.

### Asymptotic Behavior

These are some properties of a fault tolerant stack that Aumann and Bender proved in their work [4].

* A fault tolerant stack occupies _O_(1) more space.

* A fault tolerant stack's `push` and `pop` operations take  _O_(1) time.

* With _f_ faults, a fault tolerant stack loses at most _O_(_f_ log _f_) elements.
  * This is important because, unlike a normal stack, this does not depend on the number of elements.

### References

[1] Kim et al., "Flipping Bits in Memory Without Accessing Them: An Experimental Study of DRAM Disturbance Errors", ISCA 2014.

[2] Khan et al., "The Efficacy of Error Mitigation Techniques for DRAM Retention Failures: A Comparative Experimental Study", SIGMETRICS 2014.

[3] Liu et al., "An Experimental Study of Data Retention Behavior in Modern DRAM Devices: Implications for Retention Time Profiling Mechanisms", ISCA 2013.

[4] Aumann and Bender, "Fault Tolerant Data Structures", FCS 1996.
