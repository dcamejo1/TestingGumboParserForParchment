# Shared Testing Space for Testing Gumbo Parser 

It is important to note that the tests/code found here will not run properly on one's machine if Gumbo is not set up correctly. Refer to the Gumbo Documentation.

This repo will be short lived, and has the sole goal to serve as a shared testing space for contributors to Parchment to have a better understanding of the Gumbo Library.

Command for running the testing.cpp file (current central file for testing)
```bash
  g++ -std=c++11 testing.cpp `pkg-config --cflags --libs gumbo` -o test_program && ./test_program
```

Notable Observations so far:
 - Gumbo makes changes to an HTML file when it sees fit. For example, an HTML string will be modified by Gumbo to look like
```bash
Original:   <!DOCTYPE html><html><head><meta charset="UTF-8"><
Generated:  <html><head><meta charset="UTF-8"></meta><title>Si
```
 - 


