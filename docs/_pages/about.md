---
layout: about
title: about
permalink: /
description: #<a href="#">Affiliations</a>. Address. Contacts. Moto. Etc.

# profile:
#   align: right
#   image: prof_pic.jpg
#   address: >
#     <p>555 your office number</p>
#     <p>123 your address street</p>
#     <p>Your City, State 12345</p>

news: true  # includes a list of news items
social: true  # includes social icons at the bottom of the page
---

m code snippets:
{% highlight ocaml %}
# comment line: defines a one-line function
avg x y = (x + y) / 2

# defines a distance function
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy) # call c std sqrt math function

# factorial function with recursive call
factorial n = 
  if n < 2 then n
  else n * factorial (n-1)

# using for loop
loopprint n = 
  for i in 0..n
    printf "%d" i   # call c std io printf function
{% endhighlight %}



[github]: https://github.com/ligangwang/m
