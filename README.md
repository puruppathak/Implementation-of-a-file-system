# Implementation-of-a-file-system
Implement a file system that manages processes in your system (similar to what you see in /proc), called ProcFS

- The root directory is myproc that stores basic information of all processes’ in your system as files. Each file contain information of a process, filename is the same as process’s id. You are free to use any file format to store this information, i.e plain text, json, xml. You would get bonus point if you could create cool feature for this. For example, using multi-levels directories to better organize the processes, or keeping track of both active processes and retired processes.

- “ls myproc” command should return all processes’ names.

- Reading file would yield all information associated with the corresponding process.

Suggestion to start:

- Start the assignment as soon as possible.
- Download lastest version of FUSE: http://fuse.sourceforge.net [2]
- Run the hello example.
- Understand the hello example: 
http://sourceforge.net/apps/mediawiki/fuse/index.php?title=Hello_World [3]
- Understand this chart: http://en.wikipedia.org/wiki/File:FUSE_structure.svg [4]
- Read FUSE APIs and start implementation.



 
