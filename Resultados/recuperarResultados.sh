#!/bin/bash

#recuperar resultados gerados pelo NOAH
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA1.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA2.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA3.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA4.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA5.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA6.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA7.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA8.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA9.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsA10.txt .

#recuperar resultados gerados pelo modelo ARQ simples
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB1.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB2.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB3.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB4.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB5.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB6.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB7.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB8.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB9.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsB10.txt .


#recuperar resultados gerados pelo modelo VLC sem ARQ (ns3)

docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC1.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC2.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC3.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC4.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC5.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC6.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC7.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC8.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC9.txt .
docker cp ns3-3.25:/opt/ns-allinone-3.25/ns-3.25/resultsC10.txt .
