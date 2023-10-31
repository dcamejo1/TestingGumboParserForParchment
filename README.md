# Shared Testing Space for Testing Gumbo Parser 

It is important to note that the tests/code found here will not run properly on one's machine if Gumbo is not set up correctly. Refer to the Gumbo Documentation.

This repo will be short lived, and has the sole goal to serve as a shared testing space for contributors to Parchment to have a better understanding of the Gumbo Library.

Notable Observations so far:
- Gumbo does not handle meta tags very well. For example, 

```bash
  <html><head><meta charset="UTF-8"><title>Simple Page...
```

Turns into 

