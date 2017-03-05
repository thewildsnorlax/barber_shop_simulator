# barber_shop_simulator
Barber Shop Simulation using Semaphores

Problem Statement :-

Assume that the barbershop you visit has three chairs, three barbers, and a waiting area that has four Filmfare magazines and an unlimited supply of OS books for other customers. Due to space constraints, the total number of customers in the shop is restricted to 10. A customer should not enter the shop if it is filled to capacity with other customers. Once inside, if no barber is free then the customer grabs a Filmfare if available or an OS book otherwise. When a barber is free, the customer who has been reading Filmfare the longest is served and, if there are customers studying OS, the one who has been studying for the longest time now gets to read Filmfare. When a customer's hair-cut is finished, any barber can accept payment, but because there is only one cash register, payment is accepted for one customer at a time. The barbers divide their time between cutting hair, accepting payment, and sleeping in their chair waiting for a customer (they are not interested in either Filmfare or OS).
Customer arrival process can be simulated through keyboard. E.g. By pressing the key 'c', the main thread can initiate a new customer arrival thread. You can assume that a maximum of 50 customers are going to arrive on a fine Sunday morning.
