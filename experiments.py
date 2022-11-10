## Imports
from subprocess import Popen
import subprocess
from pathlib import Path
import matplotlib.pyplot as plt
import matplotlib
import time
import math
import os
import glob
from datetime import datetime
import argparse
from enum import Enum
import json
import multiprocessing
import random
from shutil import copyfile
import re
import mmap


#############
## Notes:
## (1) The nodes get started by a MsgStart message, which is currently sent by clients
## (2) The nodes will stop if there is no activity for 'numViews' views,
##     in the sense that no client transactions are processed, only dummy transactions
##     So for performance measurement of the nodes themselves, one can set 'numClTrans'
##     to 1, and nodes will only process dummy transactions
##     TODO: to set it to 0, I have to find a way to stop clients cleanly
## (3) View-changes on timeouts are not implemented yet, currently nodes simply stop when
##     they timeout.
##     TODO: once they are implemented, I have to find a way to stop nodes cleanly
## (4) If 'sleepTime' is too high and 'newViews' too low, then the nodes will give up before
##     they process all the clients' requests because they'll think they've been idle for too
##     long.  Therefore, at the moment for throughput vs. latency measurement, it's better to
##     set 'newViews' to 0, in which case the nodes keep going for ever (until they timeout).
##     The experiments will be stopped after 'cutOffBound' in that case.
####


## Parameters
sgxmode     = "SIM"
#sgxmode      = "HW"
srcsgx       = "source /opt/intel/sgxsdk/environment" # this is where the sdk is supposed to be installed
faults       = [1] #[1,2,4,10] #[1,2,4,10,20,30,40] #[1,2,4,6,8,10,12,14,20,30] # list of numbers of faults
#faults      = [1,10,20,30,40,50]
#faults      = [40]
repeats      = 100 #10 #50 #5 #100 #2     # number of times to repeat each experiment
repeatsL2    = 1
#
numViews     = 30     # number of views in each run
cutOffBound  = 60     # stop experiment after some time
#
numClients   = 1     # number of clients
numNonChCls  = 1     # number of clients for the non-chained versions
numChCls     = 1     # number of clients for the chained versions
numClTrans   = 1     # number of transactions sent by each clients
sleepTime    = 0     # time clients sleep between 2 sends (in microseconds)
timeout      = 5     # timeout before changing changing leader (in seconds)
#
numTrans      = 400    # number of transactions
payloadSize   = 0 #256 #0 #256 #128      # total size of a transaction
useMultiCores = True
numMakeCores  = multiprocessing.cpu_count()  # number of cores to use to make
#
runBase      = False #True
runCheap     = False #True
runQuick     = False #True
runComb      = False #True
runFree      = False #True
runOnep      = False #True
runChBase    = False #True
runChComb    = False #True
# Debug versions
runQuickDbg  = False #True
runChCombDbg = False #True
#
plotView     = True   # to plot the numbers of handling messages + network
plotHandle   = False  # to plot the numbers of just handling messages, without the network
plotCrypto   = False  # to plot the numbers of do crypto
debugPlot    = True #False  # to print debug info when plotting
showTitle    = True   # to print the title of the figure
plotThroughput = True
plotLatency    = True
expmode      = "" # "TVL"
showLegend1  = True
showLegend2  = False
plotBasic    = True
plotChained  = True
displayPlot  = True # to display a plot once it is generated
showYlabel   = True
displayApp   = "shotwell"
logScale     = True

# to recompile the code
recompile = True

# To set some plotting parameters for specific experiments
whichExp = ""

# For some experiments we start with f nodes dead
deadNodes    = False #True
# if deadNodes then we go with less views and give ourselves more time
if deadNodes:
    numViews = numViews // timeout
    cutOffBound = cutOffBound * 2

# For some experiments we remove the outliers
quantileSize = 20

# don't change, those are hard coded in the C++ code:
statsdir     = "stats"        # stats directory (don't change, hard coded in C++)
params       = "App/params.h" # (don't change, hard coded in C++)
config       = "App/config.h" # (don't change, hard coded in C++)
addresses    = "config"       # (don't change, hard coded in C++)
ipsOfNodes   = {}             # dictionnary mapping node ids to IPs

# to copy all files to AWS instances
copyAll = True
# set to True to randomize regions before assiging nodes to them (especially for low number of nodes)
randomizeRegions = False #True

## Global variables
completeRuns  = 0     # number of runs that successfully completed
abortedRuns   = 0     # number of runs that got aborted
aborted       = []    # list of aborted runs
allLocalPorts = []    # list of all port numbers used in local experiments

dateTimeObj  = datetime.now()
timestampStr = dateTimeObj.strftime("%d-%b-%Y-%H:%M:%S.%f")
pointsFile   = statsdir + "/points-" + timestampStr
abortedFile  = statsdir + "/aborted-" + timestampStr
plotFile     = statsdir + "/plot-" + timestampStr + ".png"
clientsFile  = statsdir + "/clients-" + timestampStr
tvlFile      = statsdir + "/tvl-" + timestampStr + ".png"
debugFile    = statsdir + "/debug-" + timestampStr

# Names
baseHS   = "Basic HotStuff"
cheapHS  = "Damysus-C"
quickHS  = "Damysus-A"
combHS   = "Basic-Damysus"
freeHS   = "Light-Damysus"
onepHS   = "OneP-Damysus"
baseChHS = "Chained HotStuff"
combChHS = "Chained-Damysus"

# Markers
baseMRK   = "P"
cheapMRK  = "o"
quickMRK  = "*"
combMRK   = "X"
freeMRK   = "s"
onepMRK   = "+"
baseChMRK = "d"
combChMRK = ">"

# Line styles
baseLS   = ":"
cheapLS  = "--"
quickLS  = "-."
combLS   = "-"
freeLS   = "-"
onepLS   = "-"
baseChLS = ":"
combChLS = "-"

# Markers
baseCOL   = "black"
cheapCOL  = "blue"
quickCOL  = "green"
combCOL   = "red"
freeCOL   = "purple"
onepCOL   = "brown"
baseChCOL = "darkorange"
combChCOL = "magenta"


## AWS parameters
instType = "t2.micro"
pem      = "aws.pem"

# Region - North Virginia (us-east-1)
region_USEAST1      = "us-east-1"
imageID_USEAST1     = "ami-09acfeca0b09f521f"
secGroup_USEAST1    = "sg-0266789e1c3d42c86"

# Region - Ohio (us-east-2)
region_USEAST2      = "us-east-2"
imageID_USEAST2     = "ami-0182bc3b30beedfa4"
secGroup_USEAST2    = "sg-0d89fb07bbd3d7700"
subnetID_USEAST2_1  = "subnet-bc5baad7" # us-east-2a
subnetID_USEAST2_2  = "subnet-30624c4a" # us-east-2b
subnetID_USEAST2_3  = "subnet-25891f69" # us-east-2c

# Region - North California (us-west-1)
region_USWEST1      = "us-west-1"
imageID_USWEST1     = "ami-0c9bcbfb9feca56c7"
secGroup_USWEST1    = "sg-0f7addd92bbae0cc2"

# Region - Oregon (us-west-2)
region_USWEST2      = "us-west-2"
imageID_USWEST2     = "ami-087b7bf2921249dc8"
secGroup_USWEST2    = "sg-02435aae297812913"

# Region - Singapore (ap-southeast-1)
region_APSEAST1     = "ap-southeast-1"
imageID_APSEAST1    = "ami-087f6cdc8a0780f6f"
secGroup_APSEAST1   = "sg-01de5d6a5bd5576b8"

# Region - Sydney (ap-southeast-2)
region_APSEAST2     = "ap-southeast-2"
imageID_APSEAST2    = "ami-085ea5cb0e80ebfd1"
secGroup_APSEAST2   = "sg-02b6c7b19d8c78ce6"

# Region - Ireland (eu-west-1)
region_EUWEST1      = "eu-west-1"
imageID_EUWEST1     = "ami-00477ebbb8f6355a4"
secGroup_EUWEST1    = "sg-00033137d10223166"

# Region - London (eu-west-2)
region_EUWEST2      = "eu-west-2"
imageID_EUWEST2     = "ami-00ceb682affe4d8d8"
secGroup_EUWEST2    = "sg-0da2a5dfe0d929307"

# Region - Paris (eu-west-3)
region_EUWEST3      = "eu-west-3"
imageID_EUWEST3     = "ami-0186f01a534d9ff40"
secGroup_EUWEST3    = "sg-03d07bde43685b6bf"

# Region - Frankfurt (eu-central-1)
region_EUCENT1      = "eu-central-1"
imageID_EUCENT1     = "ami-048124818d35d6e15"
secGroup_EUCENT1    = "sg-0b8b49fa3c6b6c77f"

# Region - Canada Central (ca-central-1)
region_CACENT1      = "ca-central-1"
imageID_CACENT1     = "ami-006e2b38fa3f30a8e"
secGroup_CACENT1    = "sg-0ce99bc9e1b8a252c"

# Regions around the world
WregionsNAME = "w"
Wregions = [(region_USEAST2,  imageID_USEAST2,  secGroup_USEAST2),
            (region_APSEAST2, imageID_APSEAST2, secGroup_APSEAST2),
            (region_EUWEST2,  imageID_EUWEST2,  secGroup_EUWEST2),
            (region_CACENT1,  imageID_CACENT1,  secGroup_CACENT1)]

# US regions
USregionsNAME = "us"
USregions = [(region_USEAST1, imageID_USEAST1, secGroup_USEAST1),
             (region_USEAST2, imageID_USEAST2, secGroup_USEAST2),
             (region_USWEST1, imageID_USWEST1, secGroup_USWEST1),
             (region_USWEST2, imageID_USWEST2, secGroup_USWEST2)]

# EU regions
EUregionsNAME = "eu"
EUregions = [(region_EUWEST1, imageID_EUWEST1, secGroup_EUWEST1),
             (region_EUWEST2, imageID_EUWEST2, secGroup_EUWEST2),
             (region_EUWEST3, imageID_EUWEST3, secGroup_EUWEST3),
             (region_EUCENT1, imageID_EUCENT1, secGroup_EUCENT1)]
# One region
ONEregionsNAME = "one"
ONEregions = [(region_USEAST2, imageID_USEAST2, secGroup_USEAST2)]

# All regions
ALLregionsNAME = "all"
ALLregions = [(region_USEAST1,  imageID_USEAST1,  secGroup_USEAST1),
              (region_USEAST2,  imageID_USEAST2,  secGroup_USEAST2),
              (region_USWEST1,  imageID_USWEST1,  secGroup_USWEST1),
              (region_USWEST2,  imageID_USWEST2,  secGroup_USWEST2),
              (region_EUWEST1,  imageID_EUWEST1,  secGroup_EUWEST1),
              (region_EUWEST2,  imageID_EUWEST2,  secGroup_EUWEST2),
              (region_EUWEST3,  imageID_EUWEST3,  secGroup_EUWEST3),
              (region_EUCENT1,  imageID_EUCENT1,  secGroup_EUCENT1),
              (region_APSEAST1, imageID_APSEAST1, secGroup_APSEAST1),
              (region_APSEAST2, imageID_APSEAST2, secGroup_APSEAST2),
              (region_CACENT1,  imageID_CACENT1,  secGroup_CACENT1)]

# All regions -- same as ALLregions but in a different order
ALL2regionsNAME = "all2"
ALL2regions = [(region_USEAST1,  imageID_USEAST1,  secGroup_USEAST1),
               (region_EUWEST1,  imageID_EUWEST1,  secGroup_EUWEST1),
               (region_APSEAST1, imageID_APSEAST1, secGroup_APSEAST1),
               (region_CACENT1,  imageID_CACENT1,  secGroup_CACENT1),
               (region_USEAST2,  imageID_USEAST2,  secGroup_USEAST2),
               (region_EUWEST2,  imageID_EUWEST2,  secGroup_EUWEST2),
               (region_APSEAST2, imageID_APSEAST2, secGroup_APSEAST2),
               (region_USWEST1,  imageID_USWEST1,  secGroup_USWEST1),
               (region_EUWEST3,  imageID_EUWEST3,  secGroup_EUWEST3),
               (region_USWEST2,  imageID_USWEST2,  secGroup_USWEST2),
               (region_EUCENT1,  imageID_EUCENT1,  secGroup_EUCENT1)]

## regions = (USregionsNAME, USregions)
#regions = (EUregionsNAME, EUregions)
## regions = (WregionsNAME, Wregions)
regions = (ONEregionsNAME, ONEregions)
#regions = (ALLregionsNAME, ALLregions)
## regions = (ALL2regionsNAME, ALL2regions)

sshOpt1  = "StrictHostKeyChecking=no"
sshOpt2  = "ConnectTimeout=10"

# Files
instFile  = "instances"
descrFile = "description"


## Docker parameters

runDocker  = False      # to run the code within docker contrainers
docker     = "docker"
dockerBase = "damysus"  # name of the docker container
networkLat = 0          # network latency in ms
networkVar = 0          # variation of the network latency
dockerMem  = 0          # memory used by containers (0 means no constraints)
dockerCpu  = 0          # cpus used by containers (0 means no constraints)


## Cluster parameters

clusterFile = "nodes"
clusterNet  = "damysusNet" # "bridge"


## Code

class Protocol(Enum):
    BASE      = "BASIC_BASELINE"           # basic baseline
    CHEAP     = "BASIC_CHEAP"              # Checker only
    QUICK     = "BASIC_QUICK"              # Accumulator only
    COMB      = "BASIC_CHEAP_AND_QUICK"    # Damysus (Checker + Accumulator)
    FREE      = "BASIC_FREE"               # hash & signature-free Damysus
    ONEP      = "BASIC_ONEP"               # 1+1/2 phase Damysus
    CHBASE    = "CHAINED_BASELINE"         # chained baseline
    CHCOMB    = "CHAINED_CHEAP_AND_QUICK"  # chained Damysus
    ## Debug versions
    QUICKDBG  = "BASIC_QUICK_DEBUG"
    CHCOMBDBG = "CHAINED_CHEAP_AND_QUICK_DEBUG" # chained Damysus - debug version


## generates a local config file
def genLocalConf(n,filename):
    open(filename, 'w').close()
    host = "127.0.0.1"

    global allLocalPorts

    print("ips:" , ipsOfNodes)

    f = open(filename,'a')
    for i in range(n):
        host  = ipsOfNodes.get(i,host)
        rport = 8760+i
        cport = 9760+i
        allLocalPorts.append(rport)
        allLocalPorts.append(cport)
        f.write("id:"+str(i)+" host:"+host+" port:"+str(rport)+" port:"+str(cport)+"\n")
    f.close()
# End of genLocalConf


def findPublicDnsName(j):
    for res in j["Reservations"]:
        for inst in res["Instances"]:
            priv   = inst["PrivateIpAddress"]
            pub    = inst["PublicIpAddress"]
            dns    = inst["PublicDnsName"]
            status = inst["State"]["Name"]
            if status == "running":
                return (priv,pub,dns)
            else:
                RuntimeError('instance is not yet running')
    raise RuntimeError('Failed to find public dns name')


def getPublicDnsName(region,i):
    while True:
        try:
            g = open(descrFile,'w')
            subprocess.run(["aws","ec2","describe-instances","--region",region,"--instance-ids",i], stdout=g)
            g.close()

            g = open(descrFile,'r')
            output = json.load(g)
            #print(output)
            g.close()

            (priv,pub,dns) = findPublicDnsName(output)
            return (priv,pub,dns)

            # g = open(descrFile,'w')
            # subprocess.run(["aws","ssm","get-connection-status","--target",i], stdout=g)
            # g.close()

            # g = open(descrFile,'r')
            # output = json.load(g)
            # g.close()

            # if output["Status"] == "connected":
            #     return (priv,pub,dns)
            # else:
            #     print("oops, not yet connected:", i)
        except KeyError:
            print("oops, cannot get address yet:", i)
            time.sleep(1)
        except RuntimeError as e:
            print("oops, error:", i, e.args)
            time.sleep(1)



def startInstances(numRepInstances,numClInstances):
    print(">> starting",str(numRepInstances),"replica instance(s)")
    print(">> starting",str(numClInstances),"client instance(s)")

    regs = regions[1]
    if randomizeRegions:
        random.shuffle(regs)

    numInstances = numRepInstances + numClInstances
    numRegions = min(numInstances,len(regs))
    k, r = divmod(numInstances,numRegions)
    #print(str(numInstances),str(numRegions),str(k),str(r))
    allInstances = []

    print("all regions:", str(regs[0:numRegions]))
    for i in range(numRegions):
        iFile = instFile + str(i)

        f = open(iFile,'w')
        reg = regs[i]
        (region,imageID,secGroup) = reg
        count = k+1 if i >= numRegions - r else k # the last r regions all run 1 more instance
        print("starting", str(count), "instance(s) here:", str(reg))
        #subprocess.run(["aws","ec2","run-instances","--region",region,"--image-id",imageID,"--count",str(numRepInstances+numClInstances),"--instance-type",instType,"--security-group-ids",secGroup,"--subnet-id",subnetID1_1], stdout=f)
        #subprocess.run(["aws","ec2","run-instances","--region",region,"--image-id",imageID,"--count",str(numRepInstances+numClInstances),"--instance-type",instType,"--security-group-ids",secGroup], stdout=f)
        print("aws ec2 run-instances --region " + region + " --image-id " + imageID + " --count " + str(count) + " --instance-type " + instType + " --security-group-ids " + secGroup)
        subprocess.run(["aws","ec2","run-instances","--region",region,"--image-id",imageID,"--count",str(count),"--instance-type",instType,"--security-group-ids",secGroup], stdout=f)
        f.close()

        f = open(iFile,'r')
        instances = json.load(f)
        allInstances.append((region,instances))
        f.close()

    # we erase the content of the file
    open(addresses,'w').close()

    # List of quadruples generated when lauching AWS EC2 instances:
    #   id/private ip/public ip/public dns
    instanceRepIds = []
    instanceClIds  = []

    n = 0 # total number of instances
    r = 0 # number of replicas
    c = 0 # number of clients
    for (region,instances) in allInstances:
        for inst in instances["Instances"]:
            id = inst["InstanceId"]
            (priv,pub,dns) = getPublicDnsName(region,id)
            print("public dns name:",dns)
            if n < numRepInstances:
                instanceRepIds.append((r,id,priv,pub,dns,region))
                h = open(addresses,'a')
                rport = 8760+r
                cport = 9760+r
                #h.write("id:"+str(r)+" host:"+str(priv)+" port:"+str(rport)+" port:"+str(cport)+"\n")
                h.write("id:"+str(r)+" host:"+str(pub)+" port:"+str(rport)+" port:"+str(cport)+"\n")
                h.close()
                r += 1
            else:
                instanceClIds.append((c,id,priv,pub,dns,region))
                c += 1

            n += 1

    if not(n == (numRepInstances + numClInstances)):
        raise RuntimeError("incorrect number of instances started", n)

    return (instanceRepIds, instanceClIds)
# End of startInstances


def copyToAddr(sshAdr):
    s1  = "scp -i " + pem + " -o " + sshOpt1 + " "
    s2  = " " + sshAdr+":/home/ubuntu/xhotstuff/"
    scp = "until " + s1 + addresses + s2 + "; do sleep 1; done"
    subprocess.run(scp, shell=True, check=True)
    if copyAll:
        subprocess.run("tar cvzf App.tar.gz --exclude='*.o' App",          shell=True, check=True)
        subprocess.run("tar cvzf Enclave.tar.gz --exclude='*.o' Enclave",  shell=True, check=True)
        subprocess.run(s1 + "Makefile"         + s2 + "",  shell=True, check=True)
        subprocess.run(s1 + "App.tar.gz"       + s2 + "",  shell=True, check=True)
        subprocess.run(s1 + "Enclave.tar.gz"   + s2 + "",  shell=True, check=True)
        cmd = "\"\"cd xhotstuff && tar xvzf App.tar.gz && tar xvzf Enclave.tar.gz\"\"" # && make clean
        p = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        p.communicate()
    else:
        subprocess.run(["scp","-i",pem,"-o",sshOpt1,params,sshAdr+":/home/ubuntu/xhotstuff/App/"])


def copyToInstances(instances):
    procs = []
    for (n,i,priv,pub,dns,region) in instances:
        sshAdr = "ubuntu@" + dns
        p = multiprocessing.Process(target=copyToAddr(sshAdr))
        p.start()
        procs.append(p)
    for p in procs:
        p.join()
# End of copyToInstances


def makeInstances(instanceIds,protocol):
    ncores = 1
    if useMultiCores:
        ncores = numMakeCores
    print(">> making",str(len(instanceIds)),"instance(s) using",str(ncores),"core(s)")

    procs  = []
    make0  = "make -j "+str(ncores)
    make   = make0 + " SGX_MODE="+sgxmode if needsSGX(protocol) else make0 + " server client"

    # copying
    #copyToInstances(instanceIds)
    # then making
    for (n,i,priv,pub,dns,region) in instanceIds:
        sshAdr = "ubuntu@" + dns
        subprocess.run(["scp","-i",pem,"-o",sshOpt1,params,sshAdr+":/home/ubuntu/xhotstuff/App/"])
        #copyToAddr(sshAdr)
        cmd    = "\"\"" + srcsgx + " && cd xhotstuff && make clean && " + make + "\"\""
        p      = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        print("the commandline is {}".format(p.args))
        procs.append(("R",n,i,priv,pub,dns,region,p))

    for (tag,n,i,priv,pub,dns,region,p) in procs:
        while (p.poll() is None):
            time.sleep(1)
        print("process done:",i)

    print("all instances are made")
# End of makeInstances


def copyClientStats(instanceClIds):
    for (n,i,priv,pub,dns,region) in instanceClIds:
        sshAdr = "ubuntu@" + dns
        subprocess.run(["scp","-i",pem,"-o",sshOpt1,sshAdr+":/home/ubuntu/xhotstuff/stats/*","stats/"])
# End of copyClientStats


def executeInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance):
    print(">> connecting to",str(len(instanceRepIds)),"replica instance(s)")
    print(">> connecting to",str(len(instanceClIds)),"client instance(s)")

    procsRep   = []
    procsCl    = []
    newtimeout = int(math.ceil(timeout+math.log(numFaults,2)))
    server     = "./sgxserver" if needsSGX(protocol) else "./server"
    client     = "./sgxclient" if needsSGX(protocol) else "./client"

    for (n,i,priv,pub,dns,region) in instanceRepIds:
        # we give some time for the nodes to connect gradually
        if (n%10 == 5):
            time.sleep(2)
        sshAdr = "ubuntu@" + dns
        srun2  = server + " " + str(n) + " " + str(numFaults) + " " + str(constFactor) + " " + str(numViews) + " " + str(newtimeout)
        srun   = "screen -d -m " + srun2
        cmd    = "\"\"" + srcsgx + " && cd xhotstuff && rm -f stats/* && " + srun2 + "\"\""
        p      = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        print("the commandline is {}".format(p.args))
        procsRep.append(("R",n,i,priv,pub,dns,region,p))

    print("started", len(procsRep), "replicas")

    # we give some time for the replicas to connect before starting the clients
    wait = 5 + int(math.ceil(math.log(numFaults,2)))
    time.sleep(wait)

    for (n,i,priv,pub,dns,region) in instanceClIds:
        sshAdr = "ubuntu@" + dns
        crun2  = client + " " + str(n) + " " + str(numFaults) + " " + str(constFactor) + " " + str(numClTrans) + " " + str(sleepTime) + " " + str(instance)
        crun   = "screen -d -m " + crun2
        cmd    = "\"\"" + srcsgx + " && cd xhotstuff && rm -f stats/* && " + crun2 + "\"\""
        p      = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        print("the commandline is {}".format(p.args))
        procsCl.append(("C",n,i,priv,pub,dns,region,p))

    print("started", len(procsCl), "clients")

    totalTime = 0

    if expmode == "TVL":
        while totalTime < cutOffBound:
            copyClientStats(instanceClIds)
            files = glob.glob(statsdir+"/client-throughput-latency-"+str(instance)+"*")
            time.sleep(1)
            totalTime += 1
            if 0 < len(files):
                print("found clients stats", files)
                for (tag,n,i,priv,pub,dns,region,p) in procsRep + procsCl:
                    p.kill()
                break
    else:
        n = 0
        # we stop processes using Python instead of inside the C++ code
        remaining = procsRep.copy()
        while 0 < len(remaining) and totalTime < cutOffBound:
            print("remaining processes at time (", totalTime, "):", remaining)
            rem = remaining.copy()
            for (tag,n,i,priv,pub,dns,region,p) in rem:
                cmdF = "find xhotstuff/" + statsdir + " -name done-" + str(n) + "* | wc -l"
                addr = "ubuntu@" + dns
                outF = int(subprocess.run("ssh -i " + pem + " -o " + sshOpt1 + " -ntt " + addr + " " + cmdF, shell=True, capture_output=True, text=True).stdout)
                #print("attempting to retrieve 'done' file for" , str(n), ":", outF)
                if 0 < int(outF):
                    print("process done:" , str(n))
                    remaining.remove((tag,n,i,priv,pub,dns,region,p))
                    n += 1
                    if (p.poll() is None):
                        p.kill()
            #time.sleep(1)
            totalTime += len(rem)
#        for (tag,n,i,priv,pub,dns,region,p) in procsRep + procsCl:
#            # We stop the execution if it takes too long (cutOffBound)
#            while (p.poll() is None) and totalTime < cutOffBound:
#                time.sleep(1)
#                totalTime += 1
#            n += 1
#            print("processes stopped:", n, "/", len(procsRep + procsCl), "-", p.args)

    global completeRuns
    global abortedRuns
    global aborted

    if totalTime < cutOffBound:
        completeRuns += 1
        print("all", len(procsRep)+len(procsCl), "all processes are done")
    else:
        abortedRuns += 1
        conf = (protocol,numFaults,instance)
        aborted.append(conf)
        f = open(abortedFile, 'a')
        f.write(str(conf)+"\n")
        f.close()
        print("------ reached cutoff bound ------")

    ## cleanup
    for (tag,n,i,priv,pub,dns,region,p) in procsRep + procsCl:
        # we print the nodes that haven't finished yet
        if (p.poll() is None):
            print("killing process still running:",(tag,n,i,priv,pub,dns,region,p.poll()))
            p.kill()
# End of executeInstances


def terminateInstance(region,i):
    while True:
        try:
            subprocess.run(["aws","ec2","terminate-instances","--region",region,"--instance-ids",i], check=True)
            print("terminated:", i)
            return True
        except CalledProcessError:
            print("oops, cannot terminate yet:", i)
            sleep(1)
# End of terminateInstance


def terminateInstances(instanceIds):
    print(">> terminating",str(len(instanceIds)),"instance(s)")
    for (n,i,priv,pub,dns,region) in instanceIds:
        terminateInstance(region,i)
# End of terminateInstances


def terminateAllInstancesRegs(regions):
    for (region,imageID,secGroup) in regions:
        f = open(instFile,'w')
        subprocess.run(["aws","ec2","describe-instances","--region",region,"--filters","Name=image-id,Values="+imageID], stdout=f)
        f.close()
        f = open(instFile,'r')
        instances = json.load(f)
        #print(instances)
        f.close()
        l = instances["Reservations"]
        print("terminating" , str(len(l)), "reservations")
        tot = 0
        for res in l:
            r = res["Instances"]
            print("terminating" , str(len(r)), "instances")
            for inst in r:
                tot += 1
                i = inst["InstanceId"]
                print(i)
                terminateInstance(region,i)
        print("terminated" , str(tot), "instances")
# End of terminateAllInstancesRegs


def terminateAllInstances():
    terminateAllInstancesRegs(regions[1])
# End of terminateAllInstances


def terminateAllInstancesAllRegs():
    terminateAllInstancesRegs(ALLregions)
# End of terminateAllInstancesAllRegs


def testAWS():
    global numMakeCores
    numMakeCores    = 1
    numRepInstances = 1
    numClInstances  = 0
    protocol        = Protocol.CHEAP
    constFactor     = 2
    numFaults       = 1
    instance        = 0

    (instanceRepIds, instanceClIds) = startInstances(numRepInstances,numClInstances)
    makeInstances(instanceRepIds+instanceClIds,protocol)
    executeInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance)
    terminateInstances(instanceRepIds + instanceClIds)
# End of testAWS


def executeAWS(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults):
    print("<<<<<<<<<<<<<<<<<<<<",
          "protocol="+protocol.value,
          ";regions="+regions[0],
          ";payload="+str(payloadSize),
          "(factor="+str(constFactor)+")",
          "#faults="+str(numFaults),
          "[complete-runs="+str(completeRuns),"aborted-runs="+str(abortedRuns)+"]")
    print("aborted runs so far:", aborted)

    numReps = (constFactor * numFaults) + 1

    print("initial number of nodes:", numReps)
    if deadNodes:
        numReps = numReps - numFaults
    print("number of nodes to actually run:", numReps)

    instanceRepIds = instanceRepIds[0:numReps]

    mkParams(protocol,constFactor,numFaults,numTrans,payloadSize)
    #time.sleep(5)
    makeInstances(instanceRepIds+instanceClIds,protocol)

    for instance in range(repeats):
        #inst = instance * instance2
        #reps = repeats * repeatsL2
        clearStatsDir()
        # execute the experiment
        executeInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance)
        # copy the stats over
        for (n,i,priv,pub,dns,region) in instanceRepIds:
            sshAdr = "ubuntu@" + dns
            subprocess.run(["scp","-i",pem,"-o",sshOpt1,sshAdr+":/home/ubuntu/xhotstuff/stats/*","stats/"])
        (throughputView,latencyView,handle,cryptoSign,cryptoVerif,cryptoNumSign,cryptoNumVerif) = computeStats(protocol,numFaults,instance,repeats)
# End of executeAWS


def runAWS():
    global numMakeCores
    numMakeCores = 1

    # Creating stats directory
    Path(statsdir).mkdir(parents=True, exist_ok=True)

    # terminating all instances
    terminateAllInstances()

    printNodePointParams()

    for numFaults in faults:
        for instance2 in range(repeatsL2):
            # starts the instances
            maxNumReps = (3 * numFaults) + 1
            (instanceRepIds, instanceClIds) = startInstances(maxNumReps,numClients)
            copyToInstances(instanceRepIds + instanceClIds)

            # ------
            # HotStuff-like baseline
            if runBase:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.BASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Cheap-HotStuff (TEE locked/prepared blocks)
            if runCheap:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.CHEAP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Quick-HotStuff (Accumulator)
            if runQuick:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.QUICK,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Quick-HotStuff (Accumulator) - debug version
            if runQuickDbg:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.QUICKDBG,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Combines Cheap&Quick-HotStuff
            if runComb:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.COMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Free
            if runFree:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.FREE,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Onep
            if runOnep:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.ONEP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Chained HotStuff-like baseline
            if runChBase:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.CHBASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Chained Cheap&Quick
            if runChComb:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.CHCOMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # Chained Cheap&Quick - debug version
            if runChCombDbg:
                executeAWS(instanceRepIds=instanceRepIds,instanceClIds=instanceClIds,protocol=Protocol.CHCOMBDBG,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
            # ------
            # We now terminate all instances just in case
            #terminateAllInstances()

            # terminates the instances
            terminateInstances(instanceRepIds + instanceClIds)

    print("num complete runs=", completeRuns)
    print("num aborted runs=", abortedRuns)
    print("aborted runs:", aborted)

    createPlot(pointsFile)
# End of runAWS


# nodes contains the nodes' information
def startRemoteContainers(nodes,numReps,numClients):
    print("running in docker mode, starting" , numReps, "containers for the replicas and", numClients, "for the clients")

    global ipsOfNodes

    lr = list(map(lambda x: (True,  x, str(x)), list(range(numReps))))            # replicas
    lc = list(map(lambda x: (False, x, "c" + str(x)), list(range(numClients))))  # clients
    lall = lr + lc

    instanceRepIds = []
    instanceClIds  = []

    for (isRep, n, i) in lall:
        #
        # we cycle through the nodes
        node  = nodes[0]
        nodes = nodes[1:]
        nodes.append(node)
        #
        # We stop and remove the Doker instance if it is still exists
        instance = dockerBase + i
        stop_cmd = docker + " stop " + instance
        rm_cmd   = docker + " rm " + instance
        sshAdr   = node["user"] + "@" + node["host"]
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,stop_cmd + "; " + rm_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
        #
        # We start the Docker instance
        # TODO: make sure to cover all the ports
        opt1 = "--expose=8000-9999"
        opt2 = "--network=" + clusterNet
        opt3 = "--cap-add=NET_ADMIN"
        opt4 = "--name " + instance
        opts = " ".join([opt1, opt2, opt3, opt4])
        run_cmd = docker + " run -td " + opts + " " + dockerBase
        s2 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,run_cmd])
        print("the commandline is {}".format(s2.args))
        s2.communicate()
        #
        exec_cmd = docker + " exec -t " + instance + " bash -c \"" + srcsgx + "; mkdir " + statsdir + "\""
        s3 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,exec_cmd])
        print("the commandline is {}".format(s3.args))
        s3.communicate()
        #
        # Set the network latency
        if 0 < networkLat:
            print("----changing network latency to " + str(networkLat) + "ms")
            tc_cmd = "tc qdisc add dev eth0 root netem delay " + str(networkLat) + "ms " + str(networkVar) + "ms distribution normal"
            lat_cmd = docker + " exec -t " + instance + " bash -c \"" + tc_cmd + "\""
            s4 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,lat_cmd])
            print("the commandline is {}".format(s4.args))
            s4.communicate()
        #
        # Extract the IP address of the container
        address = instance + "_addr"
        ip_cmd = "cd " + node["dir"] + "; " + docker + " inspect " + instance + " | jq '.[].NetworkSettings.Networks." + clusterNet + ".IPAddress' > " + address
        s5 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,ip_cmd])
        print("the commandline is {}".format(s5.args))
        s5.communicate()
        #
        s6 = Popen(["scp","-i",node["key"],"-o",sshOpt1,sshAdr+":"+node["dir"]+"/"+address,address])
        print("the commandline is {}".format(s6.args))
        s6.communicate()
        #
        rm_cmd = "cd " + node["dir"] + "; rm " + address
        s7 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,rm_cmd])
        print("the commandline is {}".format(s7.args))
        s7.communicate()
        #
        with open(address, 'r') as f:
            data = f.read()
            #print(data)
            srch = re.search('\"(.+?)\"', data)
            if srch:
                out = srch.group(1)
                print("----container's address:" + out)
                if isRep:
                    ipsOfNodes.update({n:out})
                    instanceRepIds.append((n,i,node))
                else:
                    instanceClIds.append((n,i,node))
            else:
                print("----container's address: UNKNOWN")
        subprocess.run(["rm " + address], shell=True, check=True)

    genLocalConf(numReps,addresses)

    for (n,i,node) in instanceRepIds + instanceClIds:
        #
        dockerInstance = dockerBase + i
        sshAdr = node["user"] + "@" + node["host"]
        #
        s1 = Popen(["scp","-i",node["key"],"-o",sshOpt1,addresses,sshAdr+":"+node["dir"]+"/"+addresses])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
        #
        cp_cmd = docker + " cp " + node["dir"]+"/"+addresses + " " + dockerInstance + ":/app/"
        s2 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,cp_cmd])
        print("the commandline is {}".format(s2.args))
        s2.communicate()

    return (instanceRepIds, instanceClIds)
## End of startRemoteContainers


def makeCluster(instanceIds,protocol):
    ncores = 1
    if useMultiCores:
        ncores = numMakeCores
    print(">> making",str(len(instanceIds)),"instance(s) using",str(ncores),"core(s)")

    procs  = []
    make0  = "make -j "+str(ncores)
    make   = make0 + " SGX_MODE="+sgxmode if needsSGX(protocol) else make0 + " server client"

    for (n,i,node) in instanceIds:
        #
        dockerInstance = dockerBase + i
        sshAdr = node["user"] + "@" + node["host"]
        s1 = Popen(["scp","-i",node["key"],"-o",sshOpt1,params,sshAdr+":"+node["dir"]+"/params.h"])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
        #
        cp_cmd = docker + " cp " + node["dir"]+"/params.h" + " " + dockerInstance + ":/app/App/"
        s2 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,cp_cmd])
        print("the commandline is {}".format(s2.args))
        s2.communicate()
        #
        make_cmd = docker + " exec -t " + dockerInstance + " bash -c \"" + srcsgx + "; make clean; " + make + "\""
        s3 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,make_cmd])
        print("the commandline is {}".format(s3.args))
        #s3.communicate()
        procs.append((n,i,node,s3))

    for (n,i,node,p) in procs:
        while (p.poll() is None):
            time.sleep(1)
        print("process done:",i)

    print("all instances are made")
# End of makeCluster


def executeClusterInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance):
    print(">> connecting to",str(len(instanceRepIds)),"replica instance(s)")
    print(">> connecting to",str(len(instanceClIds)),"client instance(s)")

    procsRep   = []
    procsCl    = []
    newtimeout = int(math.ceil(timeout+math.log(numFaults,2)))
    server     = "./sgxserver" if needsSGX(protocol) else "./server"
    client     = "./sgxclient" if needsSGX(protocol) else "./client"

    for (n,i,node) in instanceRepIds:
        # we give some time for the nodes to connect gradually
        if (n%10 == 5):
            time.sleep(2)
        dockerI = dockerBase + i
        sshAdr  = node["user"] + "@" + node["host"]
        srun    = " ".join([server,str(n),str(numFaults),str(constFactor),str(numViews),str(newtimeout)])
        run_cmd = docker + " exec -t " + dockerI + " bash -c \"" + srcsgx + "; rm -f stats/*; " + srun + "\""
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,run_cmd])
        print("the commandline is {}".format(s1.args))
        #s1.communicate()
        procsRep.append(("R",n,i,node,s1))

    print("started", len(procsRep), "replicas")

    # we give some time for the replicas to connect before starting the clients
    wait = 5 + int(math.ceil(math.log(numFaults,2)))
    time.sleep(wait)

    for (n,i,node) in instanceClIds:
        dockerI = dockerBase + i
        sshAdr  = node["user"] + "@" + node["host"]
        crun    = " ".join([client,str(n),str(numFaults),str(constFactor),str(numClTrans),str(sleepTime),str(instance)])
        run_cmd = docker + " exec -t " + dockerI + " bash -c \"" + srcsgx + "; rm -f stats/*; " + crun + "\""
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,run_cmd])
        print("the commandline is {}".format(s1.args))
        #s1.communicate()
        procsCl.append(("C",n,i,node,s1))

    print("started", len(procsCl), "clients")

    totalTime = 0

    if expmode == "TVL":
        print("TO FIX: TVL option")
        ## TODO
        # while totalTime < cutOffBound:
        #     copyClientStats(instanceClIds)
        #     files = glob.glob(statsdir+"/client-throughput-latency-"+str(instance)+"*")
        #     time.sleep(1)
        #     totalTime += 1
        #     if 0 < len(files):
        #         print("found clients stats", files)
        #         for (tag,n,i,priv,pub,dns,region,p) in procsRep + procsCl:
        #             p.kill()
        #         break
    else:
        remaining = procsRep.copy()
        # We wait here for all processes to complete
        # but we stop the execution if it takes too long (cutOffBound)
        while 0 < len(remaining) and totalTime < cutOffBound:
            print("remaining processes:", remaining)
            # We filter out the ones that are done. x is of the form (t,i,p)
            rem = remaining.copy()
            for (tag,n,i,node,p) in rem:
                sshAdr    = node["user"] + "@" + node["host"]
                dockerI   = dockerBase + str(i)
                find_done = "find /app/" + statsdir + " -name done-" + str(i) + "* | wc -l"
                doneFile  = "done" + str(i)
                find_cmd  = "cd " + node["dir"] + "; " + docker + " exec -t " + dockerI + " bash -c \"" + find_done + "\" > " + doneFile
                s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,find_cmd])
                print("the commandline is {}".format(s1.args))
                s1.communicate()
                #
                s2 = Popen(["scp","-i",node["key"],"-o",sshOpt1,sshAdr+":"+node["dir"]+"/"+doneFile,doneFile])
                print("the commandline is {}".format(s2.args))
                s2.communicate()
                #
                rm_cmd = "cd " + node["dir"] + "; rm " + doneFile
                s3 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,rm_cmd])
                print("the commandline is {}".format(s3.args))
                s3.communicate()
                with open(doneFile, 'r') as f:
                    out = f.read()
                    print("******" + out + "******")
                    if 0 < int(out):
                        remaining.remove((tag,n,i,node,p))
                subprocess.run(["rm " + doneFile], shell=True, check=True)
            time.sleep(1)
            totalTime += 1

    global completeRuns
    global abortedRuns
    global aborted

    if totalTime < cutOffBound:
        completeRuns += 1
        print("all", len(procsRep)+len(procsCl), "processes are done")
    else:
        abortedRuns += 1
        conf = (protocol,numFaults,instance)
        aborted.append(conf)
        f = open(abortedFile, 'a')
        f.write(str(conf)+"\n")
        f.close()
        print("------ reached cutoff bound ------")


    ## cleanup
    # kill python subprocesses
    for (tag,n,i,node,p) in procsRep + procsCl:
        # we print the nodes that haven't finished yet
        if (p.poll() is None):
            print("still running:",(tag,n,i,node,p.poll()))
            p.kill()

    ports = " ".join(list(map(lambda port: str(port) + "/tcp", allLocalPorts)))

    # we kill the processes & copy+remove the stats file to this machine
    for (tag,n,i,node,p) in procsRep + procsCl:
        sshAdr   = node["user"] + "@" + node["host"]
        dockerI  = dockerBase + i
        #
        kill_all = "killall -q sgxserver sgxclient server client; fuser -k " + ports
        kill_cmd = docker + " exec -t " + dockerI + " bash -c \"" + kill_all + "\""
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,kill_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
        #
        src = dockerI + ":/app/" + statsdir + "/."
        dst = statsdir + "/"
        cp_cmd = "cd " + node["dir"] + "; mkdir " + statsdir + "; " + docker + " cp " + src + " " + dst
        s2 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,cp_cmd])
        print("the commandline is {}".format(s2.args))
        s2.communicate()
        #
        subprocess.run(["scp","-i",node["key"],"-o",sshOpt1,sshAdr+":"+node["dir"]+"/stats/*","stats/"])
        #
        rcmd = "rm /app/" + statsdir + "/*"
        docker_rm_cmd = docker + " exec -t " + dockerI + " bash -c \"" + rcmd + "\""
        rm_cmd = "cd " + node["dir"] + "; rm -Rf " + statsdir + "; " + docker_rm_cmd
        s3 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,rm_cmd])
        print("the commandline is {}".format(s3.args))
        s3.communicate()
# End of executeClusterInstances


def executeCluster(info,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults):
    print("<<<<<<<<<<<<<<<<<<<<",
          "protocol="+protocol.value,
          ";payload="+str(payloadSize),
          "(factor="+str(constFactor)+")",
          "#faults="+str(numFaults),
          "[complete-runs="+str(completeRuns),"aborted-runs="+str(abortedRuns)+"]")
    print("aborted runs so far:", aborted)

    numReps = (constFactor * numFaults) + 1

    print("initial number of nodes:", numReps)
    if deadNodes:
        numReps = numReps - numFaults
    print("number of nodes to actually run:", numReps)

    # starts the containers
    (instanceRepIds,instanceClIds) = startRemoteContainers(info["nodes"],numReps,numClients)
    mkParams(protocol,constFactor,numFaults,numTrans,payloadSize)
    # make all nodes
    makeCluster(instanceRepIds+instanceClIds,protocol)

    for instance in range(repeats):
        clearStatsDir()
        # execute the experiment
        executeClusterInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance)
        (throughputView,latencyView,handle,cryptoSign,cryptoVerif,cryptoNumSign,cryptoNumVerif) = computeStats(protocol,numFaults,instance,repeats)

    for (n,i,node) in instanceRepIds + instanceClIds:
        instance = dockerBase + i
        stop_cmd = docker + " stop " + instance
        rm_cmd   = docker + " rm " + instance
        sshAdr   = node["user"] + "@" + node["host"]
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,stop_cmd + "; " + rm_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
# End of executeCluster


def runCluster():
    global numMakeCores
    nuMakeCores = 1

    # Creating stats directory
    Path(statsdir).mkdir(parents=True, exist_ok=True)

    printNodePointParams()

    f = open(clusterFile,'r')
    info = json.load(f)
    f.close()

    nodes = info["nodes"]

    init_cmd  = docker + " swarm init"
    leave_cmd = docker + " swarm leave --force"

    # Leave all swarms before starting
    for node in nodes:
        sshAdr = node["user"] + "@" + node["host"]
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,leave_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
    subprocess.run([leave_cmd], shell=True) #, check=True)

    srch = re.search('.*(docker swarm join .+)', subprocess.run(init_cmd, shell=True, capture_output=True, text=True).stdout)
    if srch:
        join_cmd = srch.group(1)
        print("----join command:" + join_cmd)
        for node in nodes:
            sshAdr = node["user"] + "@" + node["host"]
            s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,join_cmd])
            print("the commandline is {}".format(s1.args))
            s1.communicate()
        net_cmd = docker + " network create --driver=overlay --attachable " + clusterNet
        subprocess.run([net_cmd], shell=True, check=True)
    else:
        print("----no join command")

    for numFaults in faults:
        # ------
        # HotStuff-like baseline
        if runBase:
            executeCluster(info=info,protocol=Protocol.BASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Cheap-HotStuff (TEE locked/prepared blocks)
        if runCheap:
            executeCluster(info=info,protocol=Protocol.CHEAP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Quick-HotStuff (Accumulator)
        if runQuick:
            executeCluster(info=info,protocol=Protocol.QUICK,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Quick-HotStuff (Accumulator) - debug version
        if runQuickDbg:
            executeCluster(info=info,protocol=Protocol.QUICKDBG,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Combines Cheap&Quick-HotStuff
        if runComb:
            executeCluster(info=info,protocol=Protocol.COMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Free
        if runFree:
            executeCluster(info=info,protocol=Protocol.FREE,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Onep
        if runOnep:
            executeCluster(info=info,protocol=Protocol.ONEP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Chained HotStuff-like baseline
        if runChBase:
            executeCluster(info=info,protocol=Protocol.CHBASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Chained Cheap&Quick
        if runChComb:
            executeCluster(info=info,protocol=Protocol.CHCOMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Chained Cheap&Quick - debug version
        if runChCombDbg:
            executeCluster(info=info,protocol=Protocol.CHCOMBDBG,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)

    # cleanup
    for node in nodes:
        sshAdr = node["user"] + "@" + node["host"]
        s1 = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,leave_cmd])
        print("the commandline is {}".format(s1.args))
        s1.communicate()
    subprocess.run([leave_cmd], shell=True) #, check=True)
    subprocess.run([docker + " network rm " + clusterNet], shell=True) #, check=True)

    print("num complete runs=", completeRuns)
    print("num aborted runs=", abortedRuns)
    print("aborted runs:", aborted)

    createPlot(pointsFile)
# End of runCluster


def prepareCluster():
    f = open(clusterFile,'r')
    info = json.load(f)
    f.close()

    nodes = info["nodes"]
    procs = []
    for node in nodes:
        sshAdr = node["user"] + "@" + node["host"]
        prep_cmd = "cd " + node["dir"] + "; git clone https://github.com/vrahli/damysus.git; cd damysus; docker build -t damysus ."
        s = Popen(["ssh","-i",node["key"],"-o",sshOpt1,"-ntt",sshAdr,prep_cmd])
        procs.append((node,s))

    for (node,p) in procs:
        while (p.poll() is None):
            time.sleep(1)
        print("docker container built for node:",node["node"])
# End of prepareCluster


## Returns True if the protocol requires SGX
def needsSGX(protocol):
    if (protocol == Protocol.BASE or protocol == Protocol.CHBASE or protocol == Protocol.QUICKDBG or protocol == Protocol.CHCOMBDBG):
        return False
    else:
        return True
# End of needsSGX


def clearStatsDir():
    # Removing all (temporary) files in stats dir
    files0 = glob.glob(statsdir+"/vals*")
    files1 = glob.glob(statsdir+"/throughput-view*")
    files2 = glob.glob(statsdir+"/latency-view*")
    files3 = glob.glob(statsdir+"/handle*")
    files4 = glob.glob(statsdir+"/crypto*")
    files5 = glob.glob(statsdir+"/done*")
    files6 = glob.glob(statsdir+"/client-throughput-latency*")
    for f in files0 + files1 + files2 + files3 + files4 + files5 + files6:
        #print(f)
        os.remove(f)
# End of clearStatsDir


def mkParams(protocol,constFactor,numFaults,numTrans,payloadSize):
    f = open(params, 'w')
    f.write("#ifndef PARAMS_H\n")
    f.write("#define PARAMS_H\n")
    f.write("\n")
    f.write("#define " + protocol.value + "\n")
    f.write("#define MAX_NUM_NODES " + str((constFactor*numFaults)+1) + "\n")
    f.write("#define MAX_NUM_SIGNATURES " + str((constFactor*numFaults)+1-numFaults) + "\n")
    f.write("#define MAX_NUM_TRANSACTIONS " + str(numTrans) + "\n")
    f.write("#define PAYLOAD_SIZE " +str(payloadSize) + "\n")
    f.write("\n")
    f.write("#endif\n")
    f.close()
# End of mkParams


def mkApp(protocol,constFactor,numFaults,numTrans,payloadSize):
    ncores = 1
    if useMultiCores:
        ncores = numMakeCores
    print(">> making using",str(ncores),"core(s)")

    mkParams(protocol,constFactor,numFaults,numTrans,payloadSize)

    if runDocker:
        numReps = (constFactor * numFaults) + 1
        lr = list(map(lambda x: str(x), list(range(numReps))))           # replicas
        lc = list(map(lambda x: "c" + str(x), list(range(numClients))))  # clients
        for i in lr + lc:
            instance  = dockerBase + i
            instancex = instance + "x"
            instancey = instance
            # copying App & Enclave
            adst  = instance + ":/app/App/"
            adstx = instancex + ":/app/App/"
            adsty = adst
            edst  = instance + ":/app/Enclave/"
            edstx = instancex + ":/app/Enclave/"
            edsty = edst
            # if dockerCpu > 0 we're restricting the cpu, in which case we'll compile on the non-restricted instance and copy the files over
            if dockerCpu > 0 and dockerCpu < 1:
                instancey = instancex
                adsty = adstx
                edsty = edstx
            subprocess.run([docker + " cp Makefile "  + instancey + ":/app/"], shell=True, check=True)
            subprocess.run([docker + " cp App/. "     + adsty], shell=True, check=True)
            subprocess.run([docker + " cp Enclave/. " + edsty], shell=True, check=True)
            # DEBUG begin
            #subprocess.run(["docker cp App/Nodes.cpp " + instance + ":/app/App/"], shell=True, check=True)
            # DEBUG end
            subprocess.run([docker + " exec -t " + instancey + " bash -c \"make clean\""], shell=True, check=True)
            if needsSGX(protocol):
                subprocess.run([docker + " exec -t " + instancey + " bash -c \"" + srcsgx + "; make -j " + str(ncores) + " SGX_MODE=" + sgxmode + "\""], shell=True, check=True)
            else:
                subprocess.run([docker + " exec -t " + instancey + " bash -c \"make -j " + str(ncores) + " server client\""], shell=True, check=True)
            if dockerCpu > 0 and dockerCpu < 1:
                print("copying files from " + instancex + " to " + instance)
                tmp = "docker_tmp"
                Path(tmp).mkdir(parents=True, exist_ok=True)
                subprocess.run([docker + " cp " + instancex + ":/app/." + " " + tmp + "/"], shell=True, check=True)
                subprocess.run([docker + " cp " + tmp + "/." + " " + instance + ":/app/"], shell=True, check=True)
                #print("Files in " + instancex)
                #subprocess.run([docker + " exec -t " + instancex + " bash -c \"cd /app; ls\""], shell=True, check=True)
                #print("Files in " + tmp)
                #subprocess.run(["ls " + tmp], shell=True, check=True)
                #print("Files in " + instance)
                #subprocess.run([docker + " exec -t " + instance + " bash -c \"cd /app; ls\""], shell=True, check=True)
                #subprocess.run(["rm -Rf " + tmp], shell=True, check=True)
    else:
        subprocess.call(["make","clean"])
        if needsSGX(protocol):
            subprocess.run(["bash -c \"" + srcsgx + "\""], shell=True, check=True)
            subprocess.call(["make","-j",str(ncores),"SGX_MODE="+sgxmode])
        else:
            subprocess.call(["make","-j",str(ncores),"server","client"])
# End of mkApp


def execute(protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance):
    subsReps    = [] # list of replica subprocesses
    subsClients = [] # list of client subprocesses
    numReps = (constFactor * numFaults) + 1

    genLocalConf(numReps,addresses)

    print("initial number of nodes:", numReps)
    if deadNodes:
        numReps = numReps - numFaults
    print("number of nodes to actually run:", numReps)


    lr = list(map(lambda x: str(x), list(range(numReps))))           # replicas
    lc = list(map(lambda x: "c" + str(x), list(range(numClients))))  # clients
    lall = lr + lc

    # if running in docker mode, we copy the addresses to the containers
    if runDocker:
        for i in lall:
            dockerInstance = dockerBase + i
            dst = dockerInstance + ":/app/"
            subprocess.run([docker + " cp " + addresses + " " + dst], shell=True, check=True)

    server = "./sgxserver" if needsSGX(protocol) else "./server"
    client = "./sgxclient" if needsSGX(protocol) else "./client"

    newtimeout = int(math.ceil(timeout+math.log(numFaults,2)))
    # starting severs
    for i in range(numReps):
        # we give some time for the nodes to connect gradually
        if (i%10 == 5):
            time.sleep(2)
        cmd = " ".join([server, str(i), str(numFaults), str(constFactor), str(numViews), str(newtimeout)])
        if runDocker:
            dockerInstance = dockerBase + str(i)
            if needsSGX(protocol):
                cmd = srcsgx + "; " + cmd
            cmd = docker + " exec -t " + dockerInstance + " bash -c \"" + cmd + "\""
        p = Popen(cmd, shell=True)
        subsReps.append(("R",i,p))

    print("started", len(subsReps), "replicas")

    # starting client after a few seconds
    # TODO? instead watch the ouput from above until we've seen enough established connections
    #wait = 20 + int(math.ceil(math.log(numFaults,2)))
    wait = 5 + int(math.ceil(math.log(numFaults,2)))
    #sfact = 4 if numFaults < 2 else (3 if numFaults < 4 else (2 if numFaults < 6 else 1))
    #wait = sfact*numFaults
    time.sleep(wait)
    for cid in range(numClients):
        cmd = " ".join([client, str(cid), str(numFaults), str(constFactor), str(numClTrans), str(sleepTime), str(instance)])
        if runDocker:
            dockerInstance = dockerBase + "c" + str(cid)
            if needsSGX(protocol):
                cmd = srcsgx + "; " + cmd
            cmd = docker + " exec -t " + dockerInstance + " bash -c \"" + cmd + "\""
        c = Popen(cmd, shell=True)
        subsClients.append(("C",cid,c))

    print("started", len(subsClients), "clients")

    totalTime = 0

    if expmode == "TVL":
        remaining = subsClients.copy()
        numTotClients = len(subsClients)
        while 0 < len(remaining) and totalTime < cutOffBound:
            print(str(len(remaining)) + " remaining clients out of " + str(numTotClients) + ":", remaining)
            cFileBase = "client-throughput-latency-" + str(instance)
            if runDocker:
                rem = remaining.copy()
                for (t,i,p) in rem:
                    cFile = cFileBase + "-" + str(i) + "*"
                    dockerInstance = dockerBase + "c" + str(i)
                    cmd = "find /app/" + statsdir + " -name " + cFile + " | wc -l"
                    out = int(subprocess.run(docker + " exec -t " + dockerInstance + " bash -c \"" + cmd + "\"", shell=True, capture_output=True, text=True).stdout)
                    if 0 < int(out):
                        print("found clients stats for client ", str(i))
                        remaining.remove((t,i,p))
            else:
                remaining = list(filter(lambda x: 0 == len(glob.glob(statsdir + "/" + cFileBase + "-" + str(x[1]) + "*")), remaining))
                #files = glob.glob(statsdir+"/client-throughput-latency-"+str(instance)+"*")
                #numFiles = len(files)
            time.sleep(1)
            totalTime += 1
        for (t,i,p) in subsReps + subsClients:
            p.kill()
    else:
        remaining = subsReps.copy()
        # We wait here for all processes to complete
        # but we stop the execution if it takes too long (cutOffBound)
        while 0 < len(remaining) and totalTime < cutOffBound:
            print("remaining processes:", remaining)
            # We filter out the ones that are done. x is of the form (t,i,p)
            if runDocker:
                rem = remaining.copy()
                for (t,i,p) in rem:
                    dockerInstance = dockerBase + str(i)
                    cmd = "find /app/" + statsdir + " -name done-" + str(i) + "* | wc -l"
                    out = int(subprocess.run(docker + " exec -t " + dockerInstance + " bash -c \"" + cmd + "\"", shell=True, capture_output=True, text=True).stdout)
                    if 0 < int(out):
                        remaining.remove((t,i,p))
            else:
                remaining = list(filter(lambda x: 0 == len(glob.glob(statsdir+"/done-"+str(x[1])+"*")), remaining))
            time.sleep(1)
            totalTime += 1

    global completeRuns
    global abortedRuns
    global aborted

    if totalTime < cutOffBound:
        completeRuns += 1
        print("all", len(subsReps)+len(subsClients), "processes are done")
    else:
        abortedRuns += 1
        conf = (protocol,numFaults,instance)
        aborted.append(conf)
        f = open(abortedFile, 'a')
        f.write(str(conf)+"\n")
        f.close()
        print("------ reached cutoff bound ------")

    ## cleanup
    # kill python subprocesses
    for (t,i,p) in subsReps + subsClients:
        # we print the nodes that haven't finished yet
        if (p.poll() is None):
            print("still running:",(t,i,p.poll()))
            p.kill()

    ports = " ".join(list(map(lambda port: str(port) + "/tcp", allLocalPorts)))

    # kill processes
    if runDocker:
        # if running in docker mode, we kill the processes & copy+remove the stats file to this machine
        for i in lall:
            dockerInstance = dockerBase + i
            kcmd = "killall -q sgxserver sgxclient server client; fuser -k " + ports
            subprocess.run([docker + " exec -t " + dockerInstance + " bash -c \"" + kcmd + "\""], shell=True) #, check=True)
            #print("*** copying stat files ***")
            #subprocess.run([docker + " exec -t " + dockerInstance + " bash -c \"ls /app/" + statsdir + "\""], shell=True) #, check=True)
            src = dockerInstance + ":/app/" + statsdir + "/."
            dst = statsdir + "/"
            subprocess.run([docker + " cp " + src + " " + dst], shell=True, check=True)
            rcmd = "rm /app/" + statsdir + "/*"
            subprocess.run([docker + " exec -t " + dockerInstance + " bash -c \"" + rcmd + "\""], shell=True) #, check=True)
    else:
        subprocess.run(["killall -q sgxserver sgxclient server client; fuser -k " + ports], shell=True) #, check=True)
## End of execute


def printNodePoint(protocol,numFaults,tag,val):
    f = open(pointsFile, 'a')
    f.write("protocol="+protocol.value+" "+"faults="+str(numFaults)+" "+tag+"="+str(val)+"\n")
    f.close()
# End of printNodePoint


def printNodePointComment(protocol,numFaults,instance,repeats):
    f = open(pointsFile, 'a')
    f.write("# protocol="+protocol.value+" regions="+regions[0]+" payload="+str(payloadSize)+" faults="+str(numFaults)+" instance="+str(instance)+" repeats="+str(repeats)+"\n")
    f.close()
# End of printNodePointComment


def printNodePointParams():
    f = open(pointsFile, 'a')
    text = "##params"
    text += " cpus="+str(dockerCpu)
    text += " mem="+str(dockerMem)
    text += " lat="+str(networkLat)
    text += " payload="+str(payloadSize)
    text += " repeats1="+str(repeats)
    text += " repeats2="+str(repeatsL2)
    text += " views="+str(numViews)
    text += " regions="+regions[0]
    text += "\n"
    f.write(text)
    f.close()
# End of printNodePointParams


def computeStats(protocol,numFaults,instance,repeats):
    # Computing throughput and latency
    throughputViewVal=0.0
    throughputViewNum=0
    latencyViewVal=0.0
    latencyViewNum=0

    handleVal=0.0
    handleNum=0

    cryptoSignVal=0.0
    cryptoSignNum=0

    cryptoVerifVal=0.0
    cryptoVerifNum=0

    cryptoNumSignVal=0.0
    cryptoNumSignNum=0

    cryptoNumVerifVal=0.0
    cryptoNumVerifNum=0

    printNodePointComment(protocol,numFaults,instance,repeats)

    files = glob.glob(statsdir+"/*")
    for filename in files:
        if filename.startswith(statsdir+"/vals"):
            f = open(filename, "r")
            s = f.read()
            [thru,lat,hdl,signNum,signTime,verifNum,verifTime] = s.split(" ")

            valT = float(thru)
            throughputViewNum += 1
            throughputViewVal += valT
            printNodePoint(protocol,numFaults,"throughput-view",valT)

            valL = float(lat)
            latencyViewNum += 1
            latencyViewVal += valL
            printNodePoint(protocol,numFaults,"latency-view",valL)

            valH = float(hdl)
            handleNum += 1
            handleVal += valH
            printNodePoint(protocol,numFaults,"handle",valH)

            valST = float(signTime)
            cryptoSignNum += 1
            cryptoSignVal += valST
            printNodePoint(protocol,numFaults,"crypto-sign",valST)

            valVT = float(verifTime)
            cryptoVerifNum += 1
            cryptoVerifVal += valVT
            printNodePoint(protocol,numFaults,"crypto-verif",valVT)

            valSN = int(signNum)
            cryptoNumSignNum += 1
            cryptoNumSignVal += valSN
            printNodePoint(protocol,numFaults,"crypto-num-sign",valSN)

            valVN = int(verifNum)
            cryptoNumVerifNum += 1
            cryptoNumVerifVal += valVN
            printNodePoint(protocol,numFaults,"crypto-num-verif",valVN)

    throughputView = throughputViewVal/throughputViewNum if throughputViewNum > 0 else 0.0
    latencyView    = latencyViewVal/latencyViewNum       if latencyViewNum > 0    else 0.0
    handle         = handleVal/handleNum                 if handleNum > 0         else 0.0
    cryptoSign     = cryptoSignVal/cryptoSignNum         if cryptoSignNum > 0     else 0.0
    cryptoVerif    = cryptoVerifVal/cryptoVerifNum       if cryptoVerifNum > 0    else 0.0
    cryptoNumSign  = cryptoNumSignVal/cryptoNumSignNum   if cryptoNumSignNum > 0  else 0.0
    cryptoNumVerif = cryptoNumVerifVal/cryptoNumVerifNum if cryptoNumVerifNum > 0 else 0.0

    print("throughput-view:",  throughputView, "out of", throughputViewNum)
    print("latency-view:",     latencyView,    "out of", latencyViewNum)
    print("handle:",           handle,         "out of", handleNum)
    print("crypto-sign:",      cryptoSign,     "out of", cryptoSignNum)
    print("crypto-verif:",     cryptoVerif,    "out of", cryptoVerifNum)
    print("crypto-num-sign:",  cryptoNumSign,  "out of", cryptoNumSignNum)
    print("crypto-num-verif:", cryptoNumVerif, "out of", cryptoNumVerifNum)

    return (throughputView, latencyView, handle, cryptoSign, cryptoVerif, cryptoNumSign, cryptoNumVerif)
## End of computeStats


def startContainers(numReps,numClients):
    print("running in docker mode, starting" , numReps, "containers for the replicas and", numClients, "for the clients")

    global ipsOfNodes

    lr = list(map(lambda x: (True, str(x)), list(range(numReps))))            # replicas
    lc = list(map(lambda x: (False, "c" + str(x)), list(range(numClients))))  # clients
    lall = lr + lc

    # The 'x' containers are used in particular when we require less cpu so that we can compile in full-cpu
    # containers and copy over the code, from the x instance that does not have the restriction, which is
    # used to compile, to the non-x instance that has the restrictions
    for (isRep, i) in lall:
        instance  = dockerBase + i
        instancex = instance + "x"
        # We stop and remove the Doker instance if it is still exists
        subprocess.run([docker + " stop " + instance], shell=True) #, check=True)
        subprocess.run([docker + " stop " + instancex], shell=True) #, check=True)
        subprocess.run([docker + " rm " + instance], shell=True) #, check=True)
        subprocess.run([docker + " rm " + instancex], shell=True) #, check=True)
        # TODO: make sure to cover all the ports
        opt1  = "--expose=8000-9999"
        opt2  = "--network=\"bridge\""
        opt3  = "--cap-add=NET_ADMIN"
        opt4  = "--name " + instance
        opt4x = "--name " + instancex
        optm  = "--memory=" + str(dockerMem) + "m" if dockerMem > 0 else ""
        optc  = "--cpus=\"" + str(dockerCpu) + "\"" if dockerCpu > 0 else ""
        opts  = " ".join([opt1, opt2, opt3, opt4, optm, optc]) # with cpu/mem limitations
        optsx = " ".join([opt1, opt2, opt3, opt4x])            # without cpu/mem limitations
        # We start the Docker instance
        subprocess.run([docker + " run -td " + opts + " " + dockerBase], shell=True, check=True)
        if dockerCpu > 0 and dockerCpu < 1:
            subprocess.run([docker + " run -td " + optsx + " " + dockerBase], shell=True, check=True)
        subprocess.run([docker + " exec -t " + instance + " bash -c \"" + srcsgx + "; mkdir " + statsdir + "\""], shell=True, check=True)
        if dockerCpu > 0 and dockerCpu < 1:
            subprocess.run([docker + " exec -t " + instancex + " bash -c \"" + srcsgx + "; mkdir " + statsdir + "\""], shell=True, check=True)
        # Set the network latency
        if 0 < networkLat:
            print("----changing network latency to " + str(networkLat) + "ms")
            latcmd = "tc qdisc add dev eth0 root netem delay " + str(networkLat) + "ms " + str(networkVar) + "ms distribution normal"
            #latcmd = "tc qdisc add dev eth0 root netem delay " + str(networkLat) + "ms"
            subprocess.run([docker + " exec -t " + instance + " bash -c \"" + latcmd + "\""], shell=True, check=True)
        # Extract the IP address of the container
        ipcmd = docker + " inspect " + instance + " | jq '.[].NetworkSettings.Networks.bridge.IPAddress'"
        srch = re.search('\"(.+?)\"', subprocess.run(ipcmd, shell=True, capture_output=True, text=True).stdout)
        if srch:
            out = srch.group(1)
            print("----container's address:" + out)
            if isRep:
                ipsOfNodes.update({int(i):out})
        else:
            print("----container's address: UNKNOWN")
## End of startContainers


def stopContainers(numReps,numClients):
    print("stopping and removing docker containers")

    lr = list(map(lambda x: (True, str(x)), list(range(numReps))))            # replicas
    lc = list(map(lambda x: (False, "c" + str(x)), list(range(numClients))))  # clients
    lall = lr + lc

    for (isRep, i) in lall:
        instance = dockerBase + i
        instancex = instance + "x"
        subprocess.run([docker + " stop " + instance], shell=True) #, check=True)
        subprocess.run([docker + " stop " + instancex], shell=True) #, check=True)
        subprocess.run([docker + " rm " + instance], shell=True) #, check=True)
        subprocess.run([docker + " rm " + instancex], shell=True) #, check=True)
## End of stopContainers


# if 'recompile' is true, the application will be recompiled (default=true)
def computeAvgStats(recompile,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,numRepeats):
    print("<<<<<<<<<<<<<<<<<<<<",
          "protocol="+protocol.value,
          ";regions="+regions[0],
          ";payload="+str(payloadSize),
          "(factor="+str(constFactor)+")",
          "#faults="+str(numFaults),
          "[complete-runs="+str(completeRuns),"aborted-runs="+str(abortedRuns)+"]")
    print("aborted runs so far:", aborted)

    throughputViews=[]
    latencyViews=[]
    handles=[]
    cryptoSigns=[]
    cryptoVerifs=[]
    cryptoNumSigns=[]
    cryptoNumVerifs=[]

    numReps = (constFactor * numFaults) + 1

    if runDocker:
        startContainers(numReps,numClients)

    # building App with correct parameters
    if recompile:
        mkApp(protocol,constFactor,numFaults,numTrans,payloadSize)

    goodValues = 0

    # running 'numRepeats' time
    for i in range(numRepeats):
        print(">>>>>>>>>>>>>>>>>>>>",
              "protocol="+protocol.value,
              ";regions="+regions[0],
              ";payload="+str(payloadSize),
              "(factor="+str(constFactor)+")",
              "#faults="+str(numFaults),
              "repeat="+str(i),
              "[complete-runs="+str(completeRuns),"aborted-runs="+str(abortedRuns)+"]")
        print("aborted runs so far:", aborted)
        clearStatsDir()
        execute(protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,i)
        (throughputView,latencyView,handle,cryptoSign,cryptoVerif,cryptoNumSign,cryptoNumVerif) = computeStats(protocol,numFaults,i,numRepeats)
        if throughputView > 0 and latencyView > 0 and handle > 0 and cryptoSign > 0 and cryptoVerif > 0 and cryptoNumSign > 0 and cryptoNumVerif > 0:
            throughputViews.append(throughputView)
            latencyViews.append(latencyView)
            handles.append(handle)
            cryptoSigns.append(cryptoSign)
            cryptoVerifs.append(cryptoVerif)
            cryptoNumSigns.append(cryptoNumSign)
            cryptoNumVerifs.append(cryptoNumVerif)
            goodValues += 1

    if runDocker:
        stopContainers(numReps,numClients)

    throughputView = sum(throughputViews)/goodValues if goodValues > 0 else 0.0
    latencyView    = sum(latencyViews)/goodValues    if goodValues > 0 else 0.0
    handle         = sum(handles)/goodValues         if goodValues > 0 else 0.0
    cryptoSign     = sum(cryptoSigns)/goodValues     if goodValues > 0 else 0.0
    cryptoVerif    = sum(cryptoVerifs)/goodValues    if goodValues > 0 else 0.0
    cryptoNumSign  = sum(cryptoNumSigns)/goodValues  if goodValues > 0 else 0.0
    cryptoNumVerif = sum(cryptoNumVerifs)/goodValues if goodValues > 0 else 0.0

    print("avg throughput (view):",  throughputView)
    print("avg latency (view):",     latencyView)
    print("avg handle:",             handle)
    print("avg crypto (sign):",      cryptoSign)
    print("avg crypto (verif):",     cryptoVerif)
    print("avg crypto (sign-num):",  cryptoNumSign)
    print("avg crypto (verif-num):", cryptoNumVerif)

    return (throughputView, latencyView, handle, cryptoSign, cryptoVerif, cryptoNumSign, cryptoNumVerif)
# End of computeAvgStats


def dict2val(d,f):
    (v,n) = d.get(f)
    #print((v,n))
    return sum(v)/n


def dict2lists(d,quantileSize,p):
    faults = []
    vals   = []
    nums   = []

    # We create the lists of points from the dictionaries
    # 'val' is a list of 'num' reals
    for f,(val,num) in d.items():
        faults.append(f)
        val  = sorted(val)           # we sort the values
        l    = len(val)              # this should be num, the number of values we have in val
        n    = int(l/(100/quantileSize)) if quantileSize > 0 else 0 # we'll remove n values from the top and bottom
        newval = val[n:l-n]           # we're removing them
        #newval = val[n:l-n]          # we're removing them
        m    = len(newval)           # we're only keeping m values out of the l
        s    = sum(newval)           # we're summing up the values
        v    = s/m if m > 0 else 0.0 # and computing the average
        if p:
            print(l,quantileSize,n,v,m,"---------\n", val, "\n", newval,"\n")
        vals.append(v)
        nums.append(m)

    return (faults,vals,nums)
# End of dict2lists


## So that 3f1+1=2f2+1
def comparisonN(f1,f2,dTVBase,dTVCheap,dTVQuick,dTVComb,dTVChBase,dTVChComb,dLVBase,dLVCheap,dLVQuick,dLVComb,dLVChBase,dLVChComb):
    tv1 = dict2val(dTVBase,f1)
    tv2 = dict2val(dTVCheap,f2)
    tv3 = dict2val(dTVQuick,f2)
    tv4 = dict2val(dTVComb,f2)
    tv5 = dict2val(dTVChBase,f1)
    tv6 = dict2val(dTVChComb,f2)

    tv12 = (tv2 - tv1) / tv1 * 100
    tv13 = (tv3 - tv1) / tv1 * 100
    tv14 = (tv4 - tv1) / tv1 * 100
    tv56 = (tv6 - tv5) / tv5 * 100
    print("THROUGHPUT","cheap",tv12,"quick",tv13,"comb",tv14,"chcomb",tv56)

    lv1 = dict2val(dLVBase,f1)
    lv2 = dict2val(dLVCheap,f2)
    lv3 = dict2val(dLVQuick,f2)
    lv4 = dict2val(dLVComb,f2)
    lv5 = dict2val(dLVChBase,f1)
    lv6 = dict2val(dLVChComb,f2)

    lv12 = (lv1 - lv2) / lv1 * 100
    lv13 = (lv1 - lv3) / lv1 * 100
    lv14 = (lv1 - lv4) / lv1 * 100
    lv56 = (lv5 - lv6) / lv5 * 100
    print("LATENCY","cheap",lv12,"quick",lv13,"comb",lv14,"chcomb",lv56)
## End of comparisonN


# 'bo' should be False for throughput (increase) and True for latency (decrease)
def getPercentage(bo,nameBase,faultsBase,valsBase,nameNew,faultsNew,valsNew):
    newTot = 0.0
    newMin = 0.0
    newMax = 0.0
    newLst = []

    if faultsBase == faultsNew:
        for (n,baseVal,newVal) in zip(faultsBase,valsBase,valsNew):
            #new = (a / b) * 100 if bo else (b / a) * 100
            #print((n,baseVal,newVal))
            new = (baseVal - newVal) / baseVal * 100 if bo else (newVal - baseVal) / baseVal * 100
            newTot += new

            if new > newMax: newMax = new
            if (new < newMin or newMin == 0.0): newMin = new
            newLst.append((n,new))

    newAvg = newTot / len(faultsBase) if len(faultsBase) > 0 else 0

    print(nameNew + "/" + nameBase + "(#faults/value): " + str(newLst))
    print(nameNew + "/" + nameBase + "(avg/min/ax): " + "avg=" + str(newAvg) + ";min=" + str(newMin) + ";max=" + str(newMax))
# End of getPercentage


# From: https://stackoverflow.com/questions/7965743/how-can-i-set-the-aspect-ratio-in-matplotlib
def adjustFigAspect(fig,aspect=1):
    '''
    Adjust the subplot parameters so that the figure has the correct
    aspect ratio.
    '''
    xsize,ysize = fig.get_size_inches()
    minsize = min(xsize,ysize)
    xlim = .4*minsize/xsize
    ylim = .4*minsize/ysize
    if aspect < 1:
        xlim *= aspect
    else:
        ylim /= aspect
    fig.subplots_adjust(left=.5-xlim,
                        right=.5+xlim,
                        bottom=.5-ylim,
                        top=.5+ylim)
# End of adjustFigAspect


def createPlot(pFile):
    # throughput-view
    dictTVBase   = {}
    dictTVCheap  = {}
    dictTVQuick  = {}
    dictTVComb   = {}
    dictTVFree   = {}
    dictTVOnep   = {}
    dictTVChBase = {}
    dictTVChComb = {}
    # latency-view
    dictLVBase   = {}
    dictLVCheap  = {}
    dictLVQuick  = {}
    dictLVComb   = {}
    dictLVFree   = {}
    dictLVOnep   = {}
    dictLVChBase = {}
    dictLVChComb = {}
    # handle
    dictHBase   = {}
    dictHCheap  = {}
    dictHQuick  = {}
    dictHComb   = {}
    dictHFree   = {}
    dictHOnep   = {}
    dictHChBase = {}
    dictHChComb = {}
    # crypto-sign
    dictCSBase   = {}
    dictCSCheap  = {}
    dictCSQuick  = {}
    dictCSComb   = {}
    dictCSFree   = {}
    dictCSOnep   = {}
    dictCSChBase = {}
    dictCSChComb = {}
    # crypto-verif
    dictCVBase   = {}
    dictCVCheap  = {}
    dictCVQuick  = {}
    dictCVComb   = {}
    dictCVFree   = {}
    dictCVOnep   = {}
    dictCVChBase = {}
    dictCVChComb = {}

    global dockerCpu, dockerMem, networkLat, payloadSize, repeats, repeatsL2, numViews

    # We accumulate all the points in dictionaries
    print("reading points from:", pFile)
    f = open(pFile,'r')
    for line in f.readlines():
        if line.startswith("##params"):
            [hdr,cpu,mem,lat,payload,rep1,rep2,views,regs] = line.split(" ")
            [cpuTag,cpuVal] = cpu.split("=")
            dockerCpu = float(cpuVal)
            [memTag,memVal] = mem.split("=")
            dockerMem = int(memVal)
            [latTag,latVal] = lat.split("=")
            networkLat = int(latVal)
            [payloadTag,payloadVal] = payload.split("=")
            payloadSize = int(payloadVal)
            [rep1Tag,rep1Val] = rep1.split("=")
            repeats = int(rep1Val)
            [rep2Tag,rep2Val] = rep2.split("=")
            repeatsL2 = int(rep2Val)
            [viewsTag,viewsVal] = views.split("=")
            numViews = int(viewsVal)
            [regsTag,regsVal] = regs.split("=")
            setRegion(regsVal)

        if line.startswith("protocol"):
            [prot,faults,point]   = line.split(" ")
            [protTag,protVal]     = prot.split("=")
            [faultsTag,faultsVal] = faults.split("=")
            [pointTag,pointVal]   = point.split("=")
            numFaults=int(faultsVal)
            if float(pointVal) < float('inf'):
                # Throughputs-view
                if pointTag == "throughput-view" and protVal == "BASIC_BASELINE":
                    (val,num) = dictTVBase.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVBase.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_CHEAP":
                    (val,num) = dictTVCheap.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVCheap.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_QUICK":
                    (val,num) = dictTVQuick.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVQuick.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictTVQuick.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVQuick.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictTVComb.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVComb.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_FREE":
                    (val,num) = dictTVFree.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVFree.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_ONEP":
                    (val,num) = dictTVOnep.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVOnep.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictTVChBase.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVChBase.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictTVChComb.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVChComb.update({numFaults:(val,num+1)})
                if pointTag == "throughput-view" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictTVChComb.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictTVChComb.update({numFaults:(val,num+1)})
                # Latencies-view
                if pointTag == "latency-view" and protVal == "BASIC_BASELINE":
                    (val,num) = dictLVBase.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVBase.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_CHEAP":
                    (val,num) = dictLVCheap.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVCheap.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_QUICK":
                    (val,num) = dictLVQuick.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVQuick.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictLVQuick.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVQuick.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictLVComb.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVComb.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_FREE":
                    (val,num) = dictLVFree.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVFree.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_ONEP":
                    (val,num) = dictLVOnep.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVOnep.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictLVChBase.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVChBase.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictLVChComb.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVChComb.update({numFaults:(val,num+1)})
                if pointTag == "latency-view" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictLVChComb.get(numFaults,([],0))
                    val.append(float(pointVal))
                    dictLVChComb.update({numFaults:(val,num+1)})
                # handle
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_BASELINE":
                    (val,num) = dictHBase.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHBase.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_CHEAP":
                    (val,num) = dictHCheap.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHCheap.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_QUICK":
                    (val,num) = dictHQuick.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHQuick.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictHQuick.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHQuick.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictHComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHComb.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_FREE":
                    (val,num) = dictHFree.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHFree.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "BASIC_ONEP":
                    (val,num) = dictHOnep.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHOnep.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "CHAINED_BASELINE":
                    (val,num) = dictHChBase.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHChBase.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictHChComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHChComb.update({numFaults:(val,num+1)})
                if (pointTag == "handle" or pointTag == "latency-handle") and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictHChComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictHChComb.update({numFaults:(val,num+1)})
                # crypto-sign
                if pointTag == "crypto-sign" and protVal == "BASIC_BASELINE":
                    (val,num) = dictCSBase.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSBase.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "BASIC_CHEAP":
                    (val,num) = dictCSCheap.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSCheap.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "BASIC_QUICK":
                    (val,num) = dictCSQuick.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSQuick.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictCSQuick.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSQuick.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictCSComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSComb.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "BASIC_FREE":
                    (val,num) = dictCSFree.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSFree.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "BASIC_ONEP":
                    (val,num) = dictCSOnep.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSOnep.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictCSChBase.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSChBase.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictCSChComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSChComb.update({numFaults:(val,num+1)})
                if pointTag == "crypto-sign" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictCSChComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCSChComb.update({numFaults:(val,num+1)})
                # crypto-verif
                if pointTag == "crypto-verif" and protVal == "BASIC_BASELINE":
                    (val,num) = dictCVBase.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVBase.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "BASIC_CHEAP":
                    (val,num) = dictCVCheap.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVCheap.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "BASIC_QUICK":
                    (val,num) = dictCVQuick.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVQuick.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictCVQuick.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVQuick.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictCVComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVComb.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "BASIC_FREE":
                    (val,num) = dictCVFree.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVFree.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "BASIC_ONEP":
                    (val,num) = dictCVOnep.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVOnep.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictCVChBase.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVChBase.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictCVChComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVChComb.update({numFaults:(val,num+1)})
                if pointTag == "crypto-verif" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictCVChComb.get(numFaults,([],0))
                    val.append(float(pointVal) / numViews)
                    dictCVChComb.update({numFaults:(val,num+1)})
    f.close()

    quantileSize = 20
    quantileSize1 = 20
    quantileSize2 = 20

    # We convert the dictionaries to lists
    # throughput-view
    (faultsTVBase,   valsTVBase,   numsTVBase)   = dict2lists(dictTVBase,quantileSize,False)
    (faultsTVCheap,  valsTVCheap,  numsTVCheap)  = dict2lists(dictTVCheap,quantileSize,False)
    (faultsTVQuick,  valsTVQuick,  numsTVQuick)  = dict2lists(dictTVQuick,quantileSize,False)
    (faultsTVComb,   valsTVComb,   numsTVComb)   = dict2lists(dictTVComb,quantileSize,False)
    (faultsTVFree,   valsTVFree,   numsTVFree)   = dict2lists(dictTVFree,quantileSize,False)
    (faultsTVOnep,   valsTVOnep,   numsTVOnep)   = dict2lists(dictTVOnep,quantileSize,False)
    (faultsTVChBase, valsTVChBase, numsTVChBase) = dict2lists(dictTVChBase,quantileSize,False)
    (faultsTVChComb, valsTVChComb, numsTVChComb) = dict2lists(dictTVChComb,quantileSize,False)
    # latency-view
    (faultsLVBase,   valsLVBase,   numsLVBase)   = dict2lists(dictLVBase,quantileSize,False)
    (faultsLVCheap,  valsLVCheap,  numsLVCheap)  = dict2lists(dictLVCheap,quantileSize,False)
    (faultsLVQuick,  valsLVQuick,  numsLVQuick)  = dict2lists(dictLVQuick,quantileSize,False)
    (faultsLVComb,   valsLVComb,   numsLVComb)   = dict2lists(dictLVComb,quantileSize,False)
    (faultsLVFree,   valsLVFree,   numsLVFree)   = dict2lists(dictLVFree,quantileSize,False)
    (faultsLVOnep,   valsLVOnep,   numsLVOnep)   = dict2lists(dictLVOnep,quantileSize,False)
    (faultsLVChBase, valsLVChBase, numsLVChBase) = dict2lists(dictLVChBase,quantileSize,False)
    (faultsLVChComb, valsLVChComb, numsLVChComb) = dict2lists(dictLVChComb,quantileSize,False)
    # handle
    (faultsHBase,   valsHBase,   numsHBase)   = dict2lists(dictHBase,quantileSize1,False)
    (faultsHCheap,  valsHCheap,  numsHCheap)  = dict2lists(dictHCheap,quantileSize1,False)
    (faultsHQuick,  valsHQuick,  numsHQuick)  = dict2lists(dictHQuick,quantileSize1,False)
    (faultsHComb,   valsHComb,   numsHComb)   = dict2lists(dictHComb,quantileSize1,False)
    (faultsHFree,   valsHFree,   numsHFree)   = dict2lists(dictHFree,quantileSize1,False)
    (faultsHOnep,   valsHOnep,   numsHOnep)   = dict2lists(dictHOnep,quantileSize1,False)
    (faultsHChBase, valsHChBase, numsHChBase) = dict2lists(dictHChBase,quantileSize1,False)
    (faultsHChComb, valsHChComb, numsHChComb) = dict2lists(dictHChComb,quantileSize1,False)
    # crypto-sign
    (faultsCSBase,   valsCSBase,   numsCSBase)   = dict2lists(dictCSBase,quantileSize2,False)
    (faultsCSCheap,  valsCSCheap,  numsCSCheap)  = dict2lists(dictCSCheap,quantileSize2,False)
    (faultsCSQuick,  valsCSQuick,  numsCSQuick)  = dict2lists(dictCSQuick,quantileSize2,False)
    (faultsCSComb,   valsCSComb,   numsCSComb)   = dict2lists(dictCSComb,quantileSize2,False)
    (faultsCSFree,   valsCSFree,   numsCSFree)   = dict2lists(dictCSFree,quantileSize2,False)
    (faultsCSOnep,   valsCSOnep,   numsCSOnep)   = dict2lists(dictCSOnep,quantileSize2,False)
    (faultsCSChBase, valsCSChBase, numsCSChBase) = dict2lists(dictCSChBase,quantileSize2,False)
    (faultsCSChComb, valsCSChComb, numsCSChComb) = dict2lists(dictCSChComb,quantileSize2,False)
    # crypto-verif
    (faultsCVBase,   valsCVBase,   numsCVBase)   = dict2lists(dictCVBase,quantileSize2,False)
    (faultsCVCheap,  valsCVCheap,  numsCVCheap)  = dict2lists(dictCVCheap,quantileSize2,False)
    (faultsCVQuick,  valsCVQuick,  numsCVQuick)  = dict2lists(dictCVQuick,quantileSize2,False)
    (faultsCVComb,   valsCVComb,   numsCVComb)   = dict2lists(dictCVComb,quantileSize2,False)
    (faultsCVFree,   valsCVFree,   numsCVFree)   = dict2lists(dictCVFree,quantileSize2,False)
    (faultsCVOnep,   valsCVOnep,   numsCVOnep)   = dict2lists(dictCVOnep,quantileSize2,False)
    (faultsCVChBase, valsCVChBase, numsCVChBase) = dict2lists(dictCVChBase,quantileSize2,False)
    (faultsCVChComb, valsCVChComb, numsCVChComb) = dict2lists(dictCVChComb,quantileSize2,False)

    print("faults/throughputs(val+num)/latencies(val+num)/cypto-verif(val+num)/cypto-sign(val+num) for (baseline/cheap/quick/combined/free/onep/chained-baseline/chained-combined)")
    print((faultsTVBase,   (valsTVBase,   numsTVBase),   (valsLVBase,   numsLVBase),   (valsCVBase,   numsCVBase),   (valsCSBase,   numsCSBase)))
    print((faultsTVCheap,  (valsTVCheap,  numsTVCheap),  (valsLVCheap,  numsLVCheap),  (valsCVCheap,  numsCVCheap),  (valsCSCheap,  numsCSCheap)))
    print((faultsTVQuick,  (valsTVQuick,  numsTVQuick),  (valsLVQuick,  numsLVQuick),  (valsCVQuick,  numsCVQuick),  (valsCSQuick,  numsCSQuick)))
    print((faultsTVComb,   (valsTVComb,   numsTVComb),   (valsLVComb,   numsLVComb),   (valsCVComb,   numsCVComb),   (valsCSComb,   numsCSComb)))
    print((faultsTVFree,   (valsTVFree,   numsTVFree),   (valsLVFree,   numsLVFree),   (valsCVFree,   numsCVFree),   (valsCSFree,   numsCSFree)))
    print((faultsTVOnep,   (valsTVOnep,   numsTVOnep),   (valsLVOnep,   numsLVOnep),   (valsCVOnep,   numsCVOnep),   (valsCSOnep,   numsCSOnep)))
    print((faultsTVChBase, (valsTVChBase, numsTVChBase), (valsLVChBase, numsLVChBase), (valsCVChBase, numsCVChBase), (valsCSChBase, numsCSChBase)))
    print((faultsTVChComb, (valsTVChComb, numsTVChComb), (valsLVChComb, numsLVChComb), (valsCVChComb, numsCVChComb), (valsCSChComb, numsCSChComb)))

    print("Throughput gain (basic versions):")
    # non-chained
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,cheapHS,faultsTVCheap,valsTVCheap)
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,quickHS,faultsTVQuick,valsTVQuick)
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,combHS, faultsTVComb, valsTVComb)
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,freeHS, faultsTVFree, valsTVFree)
    getPercentage(False,combHS,faultsTVComb,valsTVComb,freeHS, faultsTVFree, valsTVFree)
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,onepHS, faultsTVOnep, valsTVOnep)
    getPercentage(False,combHS,faultsTVComb,valsTVComb,onepHS, faultsTVOnep, valsTVOnep)
    # chained
    getPercentage(False,baseChHS,faultsTVChBase,valsTVChBase,combChHS,faultsTVChComb,valsTVChComb)

    print("Latency gain (basic versions):")
    # non-chained
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,cheapHS,faultsLVCheap,valsLVCheap)
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,quickHS,faultsLVQuick,valsLVQuick)
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,combHS, faultsLVComb, valsLVComb)
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,freeHS, faultsLVFree, valsLVFree)
    getPercentage(True,combHS,faultsLVComb,valsLVComb,freeHS, faultsLVFree, valsLVFree)
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,onepHS, faultsLVOnep, valsLVOnep)
    getPercentage(True,combHS,faultsLVComb,valsLVComb,onepHS, faultsLVOnep, valsLVOnep)
    # chained
    getPercentage(True,baseChHS,faultsLVChBase,valsLVChBase,combChHS,faultsLVChComb,valsLVChComb)

    print("Handle gain (basic versions):")
    # non-chained
    getPercentage(True,baseHS,faultsHBase,valsHBase,cheapHS,faultsHCheap,valsHCheap)
    getPercentage(True,baseHS,faultsHBase,valsHBase,quickHS,faultsHQuick,valsHQuick)
    getPercentage(True,baseHS,faultsHBase,valsHBase,combHS, faultsHComb, valsHComb)
    getPercentage(True,baseHS,faultsHBase,valsHBase,freeHS, faultsHFree, valsHFree)
    getPercentage(True,combHS,faultsHComb,valsHComb,freeHS, faultsHFree, valsHFree)
    getPercentage(True,baseHS,faultsHBase,valsHBase,onepHS, faultsHOnep, valsHOnep)
    getPercentage(True,combHS,faultsHComb,valsHComb,onepHS, faultsHOnep, valsHOnep)
    # chained
    getPercentage(True,baseChHS,faultsHChBase,valsHChBase,combChHS,faultsHChComb,valsHChComb)

    LW = 1 # linewidth
    MS = 5 # markersize
    XYT = (0,5)

    #plt.figure(figsize=(2, 6))
    #, dpi=80)

    global plotThroughput
    global plotLatency

    if (plotHandle or plotCrypto) and not plotView:
        plotThroughput = False
        plotLatency    = True

    numPlots=2
    if (plotThroughput and not plotLatency) or (not plotThroughput and plotLatency):
        numPlots=1

    ## Plotting
    print("plotting",numPlots,"plot(s)")
    fig, axs = plt.subplots(numPlots,1)
    if numPlots == 1:
        x = axs
        axs = [x]
    #,figsize=(4, 10)
    if showTitle:
        if debugPlot:
            info = "file="+pFile
            info += "; cpus="+str(dockerCpu)
            info += "; mem="+str(dockerMem)
            info += "; lat="+str(networkLat)
            info += "; payload="+str(payloadSize)
            info += "; repeats1="+str(repeats)
            info += "; repeats2="+str(repeatsL2)
            info += "; #views="+str(numViews)
            info += "; regions="+regions[0]
            if plotHandle and not plotView:
                fig.suptitle("Handling time\n("+info+")")
            else:
                fig.suptitle("Throughputs (top) & Latencies (bottom)\n("+info+")")
        else:
            if plotHandle and not plotView:
                fig.suptitle("Handling time")
            else:
                fig.suptitle("Throughputs (top) & Latencies (bottom)")

    adjustFigAspect(fig,aspect=0.9)
    #adjustFigAspect(fig,aspect=0.9)
    if numPlots == 2:
        fig.set_figheight(6)
    else: # == 1
        fig.set_figheight(3)
    #fig.set_figwidth(4)

    if plotThroughput:
        # naming the x/y axis
        #axs[0].set(xlabel="#faults", ylabel="throughput")
        if showYlabel:
            axs[0].set(ylabel="throughput (Kops/s)")
        if logScale:
            axs[0].set_yscale('log')
        if whichExp == "EUexp1":
            axs[0].set_yticks((0.5,1,10,20,70))
            axs[0].set_ylim([0.5,70])
            axs[0].get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        elif whichExp == "ALLexp1":
            axs[0].set_yticks((0.3,1,6))
            axs[0].set_ylim([0.3,6])
            axs[0].get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        # giving a title to my graph
        #axs[0].set_title("throughputs")
        # plotting the points
        if plotView:
            if plotBasic:
                if len(faultsTVBase) > 0:
                    axs[0].plot(faultsTVBase,   valsTVBase,   color=baseCOL,   linewidth=LW, marker=baseMRK,   markersize=MS, linestyle=baseLS,   label=baseHS)
                if len(faultsTVCheap) > 0:
                    axs[0].plot(faultsTVCheap,  valsTVCheap,  color=cheapCOL,  linewidth=LW, marker=cheapMRK,  markersize=MS, linestyle=cheapLS,  label=cheapHS)
                if len(faultsTVQuick) > 0:
                    axs[0].plot(faultsTVQuick,  valsTVQuick,  color=quickCOL,  linewidth=LW, marker=quickMRK,  markersize=MS, linestyle=quickLS,  label=quickHS)
                if len(faultsTVComb) > 0:
                    axs[0].plot(faultsTVComb,   valsTVComb,   color=combCOL,   linewidth=LW, marker=combMRK,   markersize=MS, linestyle=combLS,   label=combHS)
                if len(faultsTVFree) > 0:
                    axs[0].plot(faultsTVFree,   valsTVFree,   color=freeCOL,   linewidth=LW, marker=freeMRK,   markersize=MS, linestyle=freeLS,   label=freeHS)
                if len(faultsTVOnep) > 0:
                    axs[0].plot(faultsTVOnep,   valsTVOnep,   color=onepCOL,   linewidth=LW, marker=onepMRK,   markersize=MS, linestyle=onepLS,   label=onepHS)
            if plotChained:
                if len(faultsTVChBase) > 0:
                    axs[0].plot(faultsTVChBase, valsTVChBase, color=baseChCOL, linewidth=LW, marker=baseChMRK, markersize=MS, linestyle=baseChLS, label=baseChHS)
                if len(faultsTVChComb) > 0:
                    axs[0].plot(faultsTVChComb, valsTVChComb, color=combChCOL, linewidth=LW, marker=combChMRK, markersize=MS, linestyle=combChLS, label=combChHS)
            if debugPlot:
                if plotBasic:
                    for x,y,z in zip(faultsTVBase, valsTVBase, numsTVBase):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsTVCheap, valsTVCheap, numsTVCheap):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsTVQuick, valsTVQuick, numsTVQuick):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsTVComb, valsTVComb, numsTVComb):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsTVFree, valsTVFree, numsTVFree):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsTVOnep, valsTVOnep, numsTVOnep):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                if plotChained:
                    for x,y,z in zip(faultsTVChBase, valsTVChBase, numsTVChBase):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsTVChComb, valsTVChComb, numsTVChComb):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')

        # legend
        if showLegend1:
            axs[0].legend(ncol=2,prop={'size': 9})

    if plotLatency:
        ax=axs[0]
        if plotThroughput:
            ax=axs[1]
        # naming the x/y axis
        if showYlabel:
            if plotHandle and not plotView:
                ax.set(xlabel="#faults", ylabel="handling time (ms)")
            else:
                ax.set(xlabel="#faults", ylabel="latency (ms)")
        else:
            ax.set(xlabel="#faults")
        if logScale:
            ax.set_yscale('log')
        if whichExp == "EUexp1":
            ax.set_yticks((5,100,600))
            ax.set_ylim([5,600])
            ax.get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        elif whichExp == "ALLexp1":
            ax.set_yticks((60,100,1000))
            ax.set_ylim([60,1000])
            ax.get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
        # giving a title to my graph
        #ax.set_title("latencies")
        # plotting the points
        if plotView:
            if plotBasic:
                if len(faultsLVBase) > 0:
                    ax.plot(faultsLVBase,   valsLVBase,   color=baseCOL,   linewidth=LW, marker=baseMRK,   markersize=MS, linestyle=baseLS,   label=baseHS)
                if len(faultsLVCheap) > 0:
                    ax.plot(faultsLVCheap,  valsLVCheap,  color=cheapCOL,  linewidth=LW, marker=cheapMRK,  markersize=MS, linestyle=cheapLS,  label=cheapHS)
                if len(faultsLVQuick) > 0:
                    ax.plot(faultsLVQuick,  valsLVQuick,  color=quickCOL,  linewidth=LW, marker=quickMRK,  markersize=MS, linestyle=quickLS,  label=quickHS)
                if len(faultsLVComb) > 0:
                    ax.plot(faultsLVComb,   valsLVComb,   color=combCOL,   linewidth=LW, marker=combMRK,   markersize=MS, linestyle=combLS,   label=combHS)
                if len(faultsLVFree) > 0:
                    ax.plot(faultsLVFree,   valsLVFree,   color=freeCOL,   linewidth=LW, marker=freeMRK,   markersize=MS, linestyle=freeLS,   label=freeHS)
                if len(faultsLVOnep) > 0:
                    ax.plot(faultsLVOnep,   valsLVOnep,   color=onepCOL,   linewidth=LW, marker=onepMRK,   markersize=MS, linestyle=onepLS,   label=onepHS)
            if plotChained:
                if len(faultsLVChBase) > 0:
                    ax.plot(faultsLVChBase, valsLVChBase, color=baseChCOL, linewidth=LW, marker=baseChMRK, markersize=MS, linestyle=baseChLS, label=baseChHS)
                if len(faultsLVChComb) > 0:
                    ax.plot(faultsLVChComb, valsLVChComb, color=combChCOL, linewidth=LW, marker=combChMRK, markersize=MS, linestyle=combChLS, label=combChHS)
            if debugPlot:
                if plotBasic:
                    for x,y,z in zip(faultsLVBase, valsLVBase, numsLVBase):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsLVCheap, valsLVCheap, numsLVCheap):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsLVQuick, valsLVQuick, numsLVQuick):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsLVComb, valsLVComb, numsLVComb):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsLVFree, valsLVFree, numsLVFree):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsLVOnep, valsLVOnep, numsLVOnep):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                if plotChained:
                    for x,y,z in zip(faultsLVChBase, valsLVChBase, numsLVChBase):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsLVChComb, valsLVChComb, numsLVChComb):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
        if plotHandle:
            if plotBasic:
                if len(faultsHBase) > 0:
                    ax.plot(faultsHBase,   valsHBase,   color=baseCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=baseLS,   label=baseHS+"")
                if len(faultsHCheap) > 0:
                    ax.plot(faultsHCheap,  valsHCheap,  color=cheapCOL,  linewidth=LW, marker="+", markersize=MS, linestyle=cheapLS,  label=cheapHS+"")
                if len(faultsHQuick) > 0:
                    ax.plot(faultsHQuick,  valsHQuick,  color=quickCOL,  linewidth=LW, marker="+", markersize=MS, linestyle=quickLS,  label=quickHS+"")
                if len(faultsHComb) > 0:
                    ax.plot(faultsHComb,   valsHComb,   color=combCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=combLS,   label=combHS+"")
                if len(faultsHFree) > 0:
                    ax.plot(faultsHFree,   valsHFree,   color=freeCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=freeLS,   label=freeHS+"")
                if len(faultsHOnep) > 0:
                    ax.plot(faultsHOnep,   valsHOnep,   color=onepCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=onepLS,   label=onepHS+"")
            if plotChained:
                if len(faultsHChBase) > 0:
                    ax.plot(faultsHChBase, valsHChBase, color=baseChCOL, linewidth=LW, marker="+", markersize=MS, linestyle=baseChLS, label=baseChHS+"")
                if len(faultsHChComb) > 0:
                    ax.plot(faultsHChComb, valsHChComb, color=combChCOL, linewidth=LW, marker="+", markersize=MS, linestyle=combChLS, label=combChHS+"")
            if debugPlot:
                if plotBasic:
                    for x,y,z in zip(faultsHBase, valsHBase, numsHBase):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsHCheap, valsHCheap, numsHCheap):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsHQuick, valsHQuick, numsHQuick):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsHComb, valsHComb, numsHComb):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsHFree, valsHFree, numsHFree):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsHOnep, valsHOnep, numsHOnep):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                if plotChained:
                    for x,y,z in zip(faultsHChBase, valsHChBase, numsHChBase):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsHChComb, valsHChComb, numsHChComb):
                        ax.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
        if plotCrypto: # Sign
            if plotBasic:
                if len(faultsCSBase) > 0:
                    axs[0].plot(faultsCSBase,   valsCSBase,   color=baseCOL,   linewidth=LW, marker="1", markersize=MS, linestyle=baseLS,   label=baseHS+" (crypto-sign)")
                if len(faultsCSCheap) > 0:
                    axs[0].plot(faultsCSCheap,  valsCSCheap,  color=cheapCOL,  linewidth=LW, marker="1", markersize=MS, linestyle=cheapLS,  label=cheapHS+" (crypto-sign)")
                if len(faultsCSQuick) > 0:
                    axs[0].plot(faultsCSQuick,  valsCSQuick,  color=quickCOL,  linewidth=LW, marker="1", markersize=MS, linestyle=quickLS,  label=quickHS+" (crypto-sign)")
                if len(faultsCSComb) > 0:
                    axs[0].plot(faultsCSComb,   valsCSComb,   color=combCOL,   linewidth=LW, marker="1", markersize=MS, linestyle=combLS,   label=combHS+" (crypto-sign)")
                if len(faultsCSFree) > 0:
                    axs[0].plot(faultsCSFree,   valsCSFree,   color=freeCOL,   linewidth=LW, marker="1", markersize=MS, linestyle=freeLS,   label=freeHS+" (crypto-sign)")
                if len(faultsCSOnep) > 0:
                    axs[0].plot(faultsCSOnep,   valsCSOnep,   color=onepCOL,   linewidth=LW, marker="1", markersize=MS, linestyle=onepLS,   label=onepHS+" (crypto-sign)")
            if plotChained:
                if len(faultsCSChBase) > 0:
                    axs[0].plot(faultsCSChBase, valsCSChBase, color=baseChCOL, linewidth=LW, marker="1", markersize=MS, linestyle=baseChLS, label=baseChHS+" (crypto-sign)")
                if len(faultsCSChComb) > 0:
                    axs[0].plot(faultsCSChComb, valsCSChComb, color=combChCOL, linewidth=LW, marker="1", markersize=MS, linestyle=combChLS, label=combChHS+" (crypto-sign)")
            if debugPlot:
                if plotBasic:
                    for x,y,z in zip(faultsCSBase, valsCSBase, numsCSBase):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCSCheap, valsCSCheap, numsCSCheap):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCSQuick, valsCSQuick, numsCSQuick):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCCSomb, valsCSComb, numsCSComb):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCSFree, valsCSFree, numsCSFree):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCSOnep, valsCSOnep, numsCSOnep):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                if plotChained:
                    for x,y,z in zip(faultsCSChBase, valsCSChBase, numsCSChBase):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCSChComb, valsCSChComb, numsCSChComb):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
        if plotCrypto: # Verif
            if plotBasic:
                if len(faultsCVBase) > 0:
                    axs[0].plot(faultsCVBase,   valsCVBase,   color=baseCOL,   linewidth=LW, marker="2", markersize=MS, linestyle=baseLS,   label=baseHS+" (crypto-verif)")
                if len(faultsCVCheap) > 0:
                    axs[0].plot(faultsCVCheap,  valsCVCheap,  color=cheapCOL,  linewidth=LW, marker="2", markersize=MS, linestyle=cheapLS,  label=cheapHS+" (crypto-verif)")
                if len(faultsCVQuick) > 0:
                    axs[0].plot(faultsCVQuick,  valsCVQuick,  color=quickCOL,  linewidth=LW, marker="2", markersize=MS, linestyle=quickLS,  label=quickHS+" (crypto-verif)")
                if len(faultsCVComb) > 0:
                    axs[0].plot(faultsCVComb,   valsCVComb,   color=combCOL,   linewidth=LW, marker="2", markersize=MS, linestyle=combLS,   label=combHS+" (crypto-verif)")
                if len(faultsCVFree) > 0:
                    axs[0].plot(faultsCVFree,   valsCVFree,   color=freeCOL,   linewidth=LW, marker="2", markersize=MS, linestyle=freeLS,   label=freeHS+" (crypto-verif)")
                if len(faultsCVOnep) > 0:
                    axs[0].plot(faultsCVOnep,   valsCVOnep,   color=onepCOL,   linewidth=LW, marker="2", markersize=MS, linestyle=onepLS,   label=onepHS+" (crypto-verif)")
            if plotChained:
                if len(faultsCVChBase) > 0:
                    axs[0].plot(faultsCVChBase, valsCVChBase, color=baseChCOL, linewidth=LW, marker="2", markersize=MS, linestyle=baseChLS, label=baseChHS+" (crypto-verif)")
                if len(faultsCVChComb) > 0:
                    axs[0].plot(faultsCVChComb, valsCVChComb, color=combChCOL, linewidth=LW, marker="2", markersize=MS, linestyle=combChLS, label=combChHS+" (crypto-verif)")
            if debugPlot:
                if plotBasic:
                    for x,y,z in zip(faultsCVBase, valsCVBase, numsCVBase):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCVCheap, valsCVCheap, numsCVCheap):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCVQuick, valsCVQuick, numsCVQuick):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCCVomb, valsCVComb, numsCVComb):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCVFree, valsCVFree, numsCVFree):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCVOnep, valsCVOnep, numsCVOnep):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                if plotChained:
                    for x,y,z in zip(faultsCVChBase, valsCVChBase, numsCVChBase):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                    for x,y,z in zip(faultsCVChComb, valsCVChComb, numsCVChComb):
                        axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
        # legend
        if showLegend2 or (showLegend1 and not plotThroughput):
            ax.legend(prop={'size': 9})

    #fig.subplots_adjust(hspace=0.5)
    fig.savefig(plotFile, bbox_inches='tight', pad_inches=0.05)
    print("points are in", pFile)
    print("plot is in", plotFile)
    if displayPlot:
        try:
            subprocess.call([displayApp, plotFile])
        except:
            print("couldn't display the plot using '" + displayApp + "'. Consider changing the 'displayApp' variable.")
    return (dictTVBase, dictTVCheap, dictTVQuick, dictTVComb, dictTVFree, dictTVOnep, dictTVChBase, dictTVChComb,
            dictLVBase, dictLVCheap, dictLVQuick, dictLVComb, dictLVFree, dictLVOnep, dictLVChBase, dictLVChComb)
# End of createPlot


def runExperiments():
    # Creating stats directory
    Path(statsdir).mkdir(parents=True, exist_ok=True)

    printNodePointParams()

    for numFaults in faults:
        # ------
        # HotStuff-like baseline
        if runBase:
            computeAvgStats(recompile,protocol=Protocol.BASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Cheap-HotStuff (TEE locked/prepared blocks)
        if runCheap:
            computeAvgStats(recompile,protocol=Protocol.CHEAP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Quick-HotStuff (Accumulator)
        if runQuick:
            computeAvgStats(recompile,protocol=Protocol.QUICK,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Quick-HotStuff (Accumulator) - debug version
        if runQuickDbg:
            computeAvgStats(recompile,protocol=Protocol.QUICKDBG,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Combines Cheap&Quick-HotStuff
        if runComb:
            computeAvgStats(recompile,protocol=Protocol.COMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Free
        if runFree:
            computeAvgStats(recompile,protocol=Protocol.FREE,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Onep
        if runOnep:
            computeAvgStats(recompile,protocol=Protocol.ONEP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Chained HotStuff-like baseline
        if runChBase:
            computeAvgStats(recompile,protocol=Protocol.CHBASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Chained Cheap&Quick
        if runChComb:
            computeAvgStats(recompile,protocol=Protocol.CHCOMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)
        # ------
        # Chained Cheap&Quick - debug version
        if runChCombDbg:
            computeAvgStats(recompile,protocol=Protocol.CHCOMBDBG,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults,numRepeats=repeats)
        else:
            (0.0,0.0,0.0,0.0)

    print("num complete runs=", completeRuns)
    print("num aborted runs=", abortedRuns)
    print("aborted runs:", aborted)

    createPlot(pointsFile)
# End of runExperiments


def printClientPoint(protocol,sleepTime,numFaults,throughput,latency,numPoints):
    f = open(clientsFile, 'a')
    f.write("protocol="+protocol.value+" "+"sleep="+str(sleepTime)+" "+"faults="+str(numFaults)+" throughput="+str(throughput)+" latency="+str(latency)+" numPoints="+str(numPoints)+"\n")
    f.close()
# End of printClientPoint


def computeClientStats(protocol,numClTrans,sleepTime,numFaults):
    throughputs = []
    latencies   = []
    files       = glob.glob(statsdir+"/*")
    for filename in files:
        if filename.startswith(statsdir+"/client-throughput-latency"):
            f = open(filename, "r")
            s = f.read()
            [thr,lat] = s.split(" ")
            throughputs.append(float(thr))
            latencies.append(float(lat))

    # we remove the top and bottom 10% quantiles
    l   = len(latencies)
    num = int(l/(100/quantileSize))

    throughputs = sorted(throughputs)
    latencies   = sorted(latencies)

    newthroughputs = throughputs[num:l-num]
    newlatencies   = latencies[num:l-num]

    throughput = 0.0
    for i in newthroughputs:
        throughput += i
    throughput = throughput/len(newthroughputs) if len(newthroughputs) > 0 else -1.0

    latency = 0.0
    for i in newlatencies:
        latency += i
    latency = latency/len(newlatencies) if len(newlatencies) > 0 else -1.0

    f = open(debugFile, 'a')
    f.write("------------------------------\n")
    f.write("numClTrans="+str(numClTrans)+";sleepTime="+str(sleepTime)+";length="+str(l)+";remove="+str(num)+";throughput="+str(throughput)+";latency="+str(latency)+"\n")
    f.write("before:\n")
    f.write(str(throughputs)+"\n")
    f.write(str(latencies)+"\n")
    f.write("after:\n")
    f.write(str(newthroughputs)+"\n")
    f.write(str(newlatencies)+"\n")
    f.close()
    print("numClTrans="+str(numClTrans)+";sleepTime="+str(sleepTime)+";length="+str(l)+";remove="+str(num)+";throughput="+str(throughput)+";latency="+str(latency))
    print("before:")
    print(throughputs)
    print(latencies)
    print("after:")
    print(newthroughputs)
    print(newlatencies)

    numPoints = l-(2*num)
    printClientPoint(protocol,sleepTime,numFaults,throughput,latency,numPoints)
# Enf of computeClientStats


def createTVLplot(cFile,instances):
    LBase = []
    TBase = []
    aBase = []

    LCheap = []
    TCheap = []
    aCheap = []

    LQuick = []
    TQuick = []
    aQuick = []

    LComb = []
    TComb = []
    aComb = []

    LFree = []
    TFree = []
    aFree = []

    LOnep = []
    TOnep = []
    aOnep = []

    LChBase = []
    TChBase = []
    aChBase = []

    LChComb = []
    TChComb = []
    aChComb = []

    print("reading points from:", cFile)
    f = open(cFile,'r')
    for line in f.readlines():
        if not line.startswith("#"):
            [prot,slp,faults,thr,lat,np] = line.split(" ")
            [protTag,protVal]     = prot.split("=")
            [sleepTag,sleepVal]   = slp.split("=")
            [faultsTag,faultsVal] = faults.split("=")
            [thrTag,thrVal]       = thr.split("=")
            [latTag,latVal]       = lat.split("=")
            [npTag,npVal]         = np.split("=") # number of points
            throughput = float(thrVal)
            latency    = float(latVal)
            sleep      = float(sleepVal)
            if protVal == "BASIC_BASELINE":
                TBase.append(throughput)
                LBase.append(latency)
                aBase.append(sleep)
            if protVal == "BASIC_CHEAP":
                TCheap.append(throughput)
                LCheap.append(latency)
                aCheap.append(sleep)
            if protVal == "BASIC_QUICK":
                TQuick.append(throughput)
                LQuick.append(latency)
                aQuick.append(sleep)
            if protVal == "BASIC_QUICK_DEBUG":
                TQuick.append(throughput)
                LQuick.append(latency)
                aQuick.append(sleep)
            if protVal == "BASIC_CHEAP_AND_QUICK":
                TComb.append(throughput)
                LComb.append(latency)
                aComb.append(sleep)
            if protVal == "BASIC_FREE":
                TFree.append(throughput)
                LFree.append(latency)
                aFree.append(sleep)
            if protVal == "BASIC_ONEP":
                TOnep.append(throughput)
                LOnep.append(latency)
                aOnep.append(sleep)
            if protVal == "CHAINED_BASELINE":
                TChBase.append(throughput)
                LChBase.append(latency)
                aChBase.append(sleep)
            if protVal == "CHAINED_CHEAP_AND_QUICK":
                TChComb.append(throughput)
                LChComb.append(latency)
                aChComb.append(sleep)
            if protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                TChComb.append(throughput)
                LChComb.append(latency)
                aChComb.append(sleep)

    LW = 1 # linewidth
    MS = 5 # markersize
    XYT = (0,5)

    #fig, ax=plt.subplots()

    plt.cla()
    plt.clf()

    ## Plotting
    print("plotting")
    if debugPlot:
        plt.title("Throughput vs. Latency\n(file="+cFile+",instances="+str(instances)+")")
    # else:
    #     plt.title("Throughput vs. Latency")

    plt.xlabel("throughput (Kops/sec)", fontsize=12)
    plt.ylabel("latency (ms)", fontsize=12)
    if plotBasic:
        if len(TBase) > 0:
            plt.plot(TBase,   LBase,   color=baseCOL,   linewidth=LW, marker=baseMRK,   markersize=MS, linestyle=baseLS,   label=baseHS)
        if len(TCheap) > 0:
            plt.plot(TCheap,  LCheap,  color=cheapCOL,  linewidth=LW, marker=cheapMRK,  markersize=MS, linestyle=cheapLS,  label=cheapHS)
        if len(TQuick) > 0:
            plt.plot(TQuick,  LQuick,  color=quickCOL,  linewidth=LW, marker=quickMRK,  markersize=MS, linestyle=quickLS,  label=quickHS)
        if len(TComb) > 0:
            plt.plot(TComb,   LComb,   color=combCOL,   linewidth=LW, marker=combMRK,   markersize=MS, linestyle=combLS,   label=combHS)
        if len(TFree) > 0:
            plt.plot(TFree,   LFree,   color=freeCOL,   linewidth=LW, marker=freeMRK,   markersize=MS, linestyle=freeLS,   label=freeHS)
        if len(TOnep) > 0:
            plt.plot(TOnep,   LOnep,   color=onepCOL,   linewidth=LW, marker=onepMRK,   markersize=MS, linestyle=onepLS,   label=onepHS)
    if plotChained:
        if len(TChBase) > 0:
            plt.plot(TChBase, LChBase, color=baseChCOL, linewidth=LW, marker=baseChMRK, markersize=MS, linestyle=baseChLS, label=baseChHS)
        if len(TChComb) > 0:
            plt.plot(TChComb, LChComb, color=combChCOL, linewidth=LW, marker=combChMRK, markersize=MS, linestyle=combChLS, label=combChHS)
    if debugPlot:
        if plotBasic:
            for x,y,z in zip(TBase, LBase, aBase):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(TCheap, LCheap, aCheap):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(TQuick, LQuick, aQuick):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(TComb, LComb, aComb):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(TFree, LFree, aFree):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(TOnep, LOnep, aOnep):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
        if plotChained:
            for x,y,z in zip(TChBase, LChBase, aChBase):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(TChComb, LChComb, aChComb):
                plt.annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')

    # Font
    plt.rcParams.update({'font.size': 12})

    # legend
    plt.legend()

    #ax.set_aspect(aspect=0.1)
    if logScale:
        plt.yscale('log')
    #plt.yscale('log',base=2)

    plt.savefig(tvlFile, bbox_inches='tight', pad_inches=0.05)
    print("plot is in", tvlFile)
    if displayPlot:
        subprocess.call(["shotwell",tvlFile])
# Enf of createTVLplot


def oneTVL(protocol,constFactor,numFaults,numTransPerBlock,payloadSize,numClTrans,numViews,cutOffBound,sleepTimes,repeats):
    numReps = (constFactor * numFaults) + 1

    if runDocker:
        startContainers(numReps,numClients)

    mkApp(protocol,constFactor,numFaults,numTransPerBlock,payloadSize)
    for sleepTime in sleepTimes:
        clearStatsDir()
        for i in range(repeats):
            print(">>>>>>>>>>>>>>>>>>>>",
                  "protocol="+protocol.value,
                  ";regions="+regions[0],
                  ";payload="+str(payloadSize),
                  "(factor="+str(constFactor)+")",
                  "sleep="+str(sleepTime),
                  "#faults="+str(numFaults),
                  "repeat="+str(i))
            time.sleep(2)
            execute(protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,i)
        computeClientStats(protocol,numClTrans,sleepTime,numFaults)

    if runDocker:
        stopContainers(numReps,numClients)
# End of oneTVL


# throuput vs. latency
def TVL():
    # Creating stats directory
    Path(statsdir).mkdir(parents=True, exist_ok=True)

    global expmode
    expmode = "TVL"

    global debugPlot
    debugPlot = True

    # Values for the non-chained versions
    #numClTrans   = 110000
    numClTrans   = 100000 #250000 #260000 #500000 # - 100000 seems fine for basic versions
    #numClients   = 5 #2 #16 #1 # --- 1 seems fine for basic versions

    # Values for the chained version
    numClTransCh = 100000 #250000 #260000 #500000 # - 100000 seems fine for basic versions
    #numClientsCh = 6 #4 #16 #1 # --- 1 seems fine for basic versions

    numFaults        = 1
    numTransPerBlock = 400 #10
    payloadSize      = 0 #256
    numViews         = 0 # nodes don't stop
    cutOffBound      = 90


    ## For testing purposes, we use less repeats then
    test = True


    if test:
        sleepTimes = [500,100,50,10,5,0]
        #sleepTimes = [900,500,50,10]
    else:
        sleepTimes = [900,700,500,100,50,10,5,0] #[500,50,0] #

    f = open(clientsFile, 'a')
    f.write("# transactions="+str(numClTrans)+" "+
            "faults="+str(numFaults)+" "+
            "transactions="+str(numTransPerBlock)+" "+
            "payload="+str(payloadSize)+" "+
            "cutoff="+str(cutOffBound)+" "+
            "repeats="+str(repeats)+" "+
            "rates="+str(sleepTimes)+"\n")
    f.close()

    ## TODO : make this a parameter instead
    global numClients
    numClients = numNonChCls

    # HotStuff-like baseline
    if runBase:
        oneTVL(protocol=Protocol.BASE,
               constFactor=3,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Cheap-HotStuff
    if runCheap:
        oneTVL(protocol=Protocol.CHEAP,
               constFactor=2,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Quick-HotStuff
    if runQuick:
        oneTVL(protocol=Protocol.QUICK,
               constFactor=3,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Quick-HotStuff - debug version
    if runQuickDbg:
        oneTVL(protocol=Protocol.QUICKDBG,
               constFactor=3,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Cheap&Quick-HotStuff
    if runComb:
        oneTVL(protocol=Protocol.COMB,
               constFactor=2,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Free
    if runFree:
        oneTVL(protocol=Protocol.FREE,
               constFactor=2,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Onep
    if runOnep:
        oneTVL(protocol=Protocol.ONEP,
               constFactor=2,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTrans,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    numClients = numChCls

    # Chained HotStuff-like baseline
    if runChBase:
        oneTVL(protocol=Protocol.CHBASE,
               constFactor=3,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTransCh,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Chained Cheap&Quick-HotStuff
    if runChComb:
        oneTVL(protocol=Protocol.CHCOMB,
               constFactor=2,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTransCh,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    # Chained Cheap&Quick-HotStuff - debug version
    if runChCombDbg:
        oneTVL(protocol=Protocol.CHCOMBDBG,
               constFactor=2,
               numFaults=numFaults,
               numTransPerBlock=numTransPerBlock,
               payloadSize=payloadSize,
               numClTrans=numClTransCh,
               numViews=numViews,
               cutOffBound=cutOffBound,
               sleepTimes=sleepTimes,
               repeats=repeats)

    createTVLplot(clientsFile,numClTrans)
    print("debug info is in", debugFile)
# End of TVL


def oneTVLaws(protocol,constFactor,numFaults,allRepIds,allClIds,numTransPerBlock,payloadSize,numCl,numClTrans,numViews,cutOffBound,sleepTimes,repeats):
    global numClients
    numClients = numCl

    numReps = (constFactor * numFaults) + 1
    instanceRepIds = allRepIds[0:numReps]
    instanceClIds = allClIds

    mkParams(protocol,constFactor,numFaults,numTransPerBlock,payloadSize)
    #time.sleep(5)
    makeInstances(instanceRepIds+instanceClIds,protocol)

    # sleepTimes holds different rates at which clients send data
    for sleepTime in sleepTimes:
        clearStatsDir()
        for i in range(repeats):
            print(">>>>>>>>>>>>>>>>>>>>",
                  "protocol="+protocol.value,
                  ";regions="+regions[0],
                  ";payload="+str(payloadSize),
                  "(factor="+str(constFactor)+")",
                  "sleep="+str(sleepTime),
                  "#faults="+str(numFaults),
                  "repeat="+str(i))
            time.sleep(2)
            executeInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,i)
        # copy the stats over
        copyClientStats(instanceClIds)
        computeClientStats(protocol,numClTrans,sleepTime,numFaults)
# End of oneTVLaws


def TVLaws():
    global numMakeCores
    numMakeCores = 1

    global expmode
    expmode = "TVL"

    global debugPlot
    debugPlot = True

    global regions
    regions = (EUregionsNAME, EUregions)

    global quantileSize
    quantileSize = 20

    ## 10 clients & 500000 was too much for Chained-HotStuff (30-Sept-2021-08 file)
    ## 10 clients & 200000 was not enough
    ## 10 clients & 250000 was not enough
    ## 10 clients & 280000 270000 a bit too much?
    ## 10 clients & 300000 seems to be fine (need to rerun) - maybe a bit too high?

    ## For testing purposes, we use less repeats then
    test = True

    # Values for the non-chained versions
    numClTrans   = 250000 #250000 #260000 #500000 # - 100000 seems fine for basic versions
    numClients   = 10 #16 #1 # --- 1 seems fine for basic versions

    # Values for the chained version
    numClTransCh = 50000 #250000 #260000 #500000 # - 100000 seems fine for basic versions
    numClientsCh = 6 #4 #16 #1 # --- 1 seems fine for basic versions

    # Common parameters
    numFaults        = 1
    numTransPerBlock = 400
    payloadSize      = 0 #256
    numViews         = 0 # nodes don't stop
    cutOffBound      = 90
    #sleepTimes  = [500,200,100,50,10,5,2,1,0]
    #sleepTimes  = [500,100,50,10,5,1,0]

    if test:
        repeats    = 2 #5 #10
        sleepTimes = [900,500,50,0] #[700,500,100,50,10,5,0] #
    else:
        repeats    = 100 #10 #20 #5 #200 #70 #50
        sleepTimes = [900,700,500,100,50,10,5,0] #[500,50,0] #

    f = open(clientsFile, 'a')
    f.write("# clientTransactions="+str(numClTrans)+" "+
            "faults="+str(numFaults)+" "+
            "transactions="+str(numTransPerBlock)+" "+
            "payload="+str(payloadSize)+" "+
            "numClients="+str(numClients)+" "+
            "cutoff="+str(cutOffBound)+" "+
            "repeats="+str(repeats)+" "+
            "rates="+str(sleepTimes)+"\n")
    f.close()


    ## Non-Chained Versions

    # the max number of replicas
    numReps = (3 * numFaults) + 1
    (allRepIds, allClIds) = startInstances(numReps,numClients)

    if runBase:
        oneTVLaws(protocol=Protocol.BASE,
                  constFactor=3,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClients,
                  numClTrans=numClTrans,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    if runCheap:
        oneTVLaws(protocol=Protocol.CHEAP,
                  constFactor=2,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClients,
                  numClTrans=numClTrans,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    if runQuick:
        oneTVLaws(protocol=Protocol.QUICK,
                  constFactor=3,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClients,
                  numClTrans=numClTrans,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    if runComb:
        oneTVLaws(protocol=Protocol.COMB,
                  constFactor=2,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClients,
                  numClTrans=numClTrans,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    if runFree:
        oneTVLaws(protocol=Protocol.FREE,
                  constFactor=2,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClients,
                  numClTrans=numClTrans,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    if runOnep:
        oneTVLaws(protocol=Protocol.ONEP,
                  constFactor=2,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClients,
                  numClTrans=numClTrans,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)


    ## Chained Versions

    if runChBase or runChComb:
        terminateInstances(allRepIds + allClIds)
        (allRepIds, allClIds) = startInstances(numReps,numClientsCh)

    if runChBase:
        oneTVLaws(protocol=Protocol.CHBASE,
                  constFactor=3,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClientsCh,
                  numClTrans=numClTransCh,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    if runChComb:
        oneTVLaws(protocol=Protocol.CHCOMB,
                  constFactor=2,
                  numFaults=numFaults,
                  allRepIds=allRepIds,
                  allClIds=allClIds,
                  numTransPerBlock=numTransPerBlock,
                  payloadSize=payloadSize,
                  numCl=numClientsCh,
                  numClTrans=numClTransCh,
                  numViews=numViews,
                  cutOffBound=cutOffBound,
                  sleepTimes=sleepTimes,
                  repeats=repeats)

    terminateInstances(allRepIds + allClIds)

    createTVLplot(clientsFile,numClTrans)
    print("debug info is in", debugFile)
# End of TVLaws


def copyLatestExperiments():
    global plotFile
    global tvlFile
    global plotBasic
    global plotChained
    global plotHandle
    global plotView
    global showYlabel
    global showLegend1
    global showLegend2
    global whichExp
    global showTitle
    global debugPlot

    showTitle = False
    debugPlot = False

    plotBasic   = True
    plotChained = True

    showYlabel  = True
    showLegend1 = True
    showLegend2 = False

    # EUregions, payload=256
    whichExp  = "EUexp1"
    pointFile = statsdir+"/points-09-Sep-2021-14:37:34.270859"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase1,dTVCheap1,dTVQuick1,dTVComb1,dTVFree1,dTVOnep1,dTVChBase1,dTVChComb1,dLVBase1,dLVCheap1,dLVQuick1,dLVComb1,dLVFree1,dLVOnep1,dLVChBase1,dLVChComb1) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-EUregs-256B.png")

    print("--THROUGHPUT/LATENCY EU256")
    comparisonN(20,30,dTVBase1,dTVCheap1,dTVQuick1,dTVComb1,dTVChBase1,dTVChComb1,dLVBase1,dLVCheap1,dLVQuick1,dLVComb1,dLVChBase1,dLVChComb1)
    print("--")

    showYlabel  = False
    showLegend1 = False
    showLegend2 = False

    # EUregions, payload=0
    whichExp  = "EUexp1"
    pointFile = statsdir+"/points-18-Sep-2021-08:40:10.075174"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase2,dTVCheap2,dTVQuick2,dTVComb2,dTVFree2,dTVOnep2,dTVChBase2,dTVChComb2,dLVBase2,dLVCheap2,dLVQuick2,dLVComb2,dLVFree2,dLVOnep2,dLVChBase2,dLVChComb2) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-EUregs-0B.png")

    print("--THROUGHPUT/LATENCY EU0")
    comparisonN(20,30,dTVBase2,dTVCheap2,dTVQuick2,dTVComb2,dTVChBase2,dTVChComb2,dLVBase2,dLVCheap2,dLVQuick2,dLVComb2,dLVChBase2,dLVChComb2)
    print("--")

    showYlabel  = True
    showLegend1 = True
    showLegend2 = False

    # ALLregions, payload=256
    whichExp  = "ALLexp1"
    pointFile = statsdir+"/points-12-Sep-2021-21:22:48.294547-v2"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase3,dTVCheap3,dTVQuick3,dTVComb3,dTVFree3,dTVOnep3,dTVChBase3,dTVChComb3,dLVBase3,dLVCheap3,dLVQuick3,dLVComb3,dLVFree3,dLVOnep3,dLVChBase3,dLVChComb3) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ALLregs-256B.png")

    print("--THROUGHPUT/LATENCY ALL256")
    comparisonN(20,30,dTVBase3,dTVCheap3,dTVQuick3,dTVComb3,dTVChBase3,dTVChComb3,dLVBase3,dLVCheap3,dLVQuick3,dLVComb3,dLVChBase3,dLVChComb3)
    print("--")

    showYlabel  = False
    showLegend1 = False
    showLegend2 = False

    # ALLregions, payload=0
    whichExp  = "ALLexp1"
    pointFile = statsdir+"/points-23-Sep-2021-20:57:01.200810-v2"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase4,dTVCheap4,dTVQuick4,dTVComb4,dTVFree4,dTVOnep4,dTVChBase4,dTVChComb4,dLVBase4,dLVCheap4,dLVQuick4,dLVComb4,dLVFree4,dLVOnep4,dLVChBase4,dLVChComb4) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ALLregs-0B.png")

    print("--THROUGHPUT/LATENCY ALL0")
    comparisonN(20,30,dTVBase4,dTVCheap4,dTVQuick4,dTVComb4,dTVChBase4,dTVChComb4,dLVBase4,dLVCheap4,dLVQuick4,dLVComb4,dLVChBase4,dLVChComb4)
    print("--")

    # TVL - EUregions, payload=0, chained
    plotBasic   = False
    plotChained = True
    clientsFile = statsdir+"/clients-04-Oct-2021-12:11:35.605162"
    tvlFile  = statsdir + "/tvl-" + timestampStr + ".png"
    createTVLplot(clientsFile,-1)
    copyfile(tvlFile,"../figures/tvl-chained-EUregs-0B.png")

    # TVL - EUregions, payload=0, basic
    plotBasic   = True
    plotChained = False
    clientsFile = statsdir+"/clients-06-Oct-2021-03:11:36.399919"
    tvlFile  = statsdir + "/tvl-" + timestampStr + ".png"
    createTVLplot(clientsFile,-1)
    copyfile(tvlFile,"../figures/tvl-basic-EUregs-0B.png")

    showYlabel  = True
    showLegend1 = True
    showLegend2 = False

    # ONEregion, payload=0
    whichExp  = "ONEexp1"
    pointFile = statsdir+"/points-08-Sep-2022-combined"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase1,dTVCheap1,dTVQuick1,dTVComb1,dTVFree1,dTVOnep1,dTVChBase1,dTVChComb1,dLVBase1,dLVCheap1,dLVQuick1,dLVComb1,dLVFree1,dLVOnep1,dLVChBase1,dLVChComb1) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ONEreg-0B.png")

    # ONEregion, payload=256
    whichExp  = "ONEexp1"
    pointFile = statsdir+"/points-16-Sep-2022-combined"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase1,dTVCheap1,dTVQuick1,dTVComb1,dTVFree1,dTVOnep1,dTVChBase1,dTVChComb1,dLVBase1,dLVCheap1,dLVQuick1,dLVComb1,dLVFree1,dLVOnep1,dLVChBase1,dLVChComb1) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ONEreg-256B.png")

    plotHandle = True
    plotView   = False

    # ONEregion, payload=0 -- handleonly
    whichExp  = "ONEexp1"
    pointFile = statsdir+"/points-08-Sep-2022-combined"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase1,dTVCheap1,dTVQuick1,dTVComb1,dTVFree1,dTVOnep1,dTVChBase1,dTVChComb1,dLVBase1,dLVCheap1,dLVQuick1,dLVComb1,dLVFree1,dLVOnep1,dLVChBase1,dLVChComb1) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ONEreg-0B-handle.png")

    # ONEregion, payload=256 --handleonly
    whichExp  = "ONEexp1"
    pointFile = statsdir+"/points-16-Sep-2022-combined"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    (dTVBase1,dTVCheap1,dTVQuick1,dTVComb1,dTVFree1,dTVOnep1,dTVChBase1,dTVChComb1,dLVBase1,dLVCheap1,dLVQuick1,dLVComb1,dLVFree1,dLVOnep1,dLVChBase1,dLVChComb1) = createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ONEreg-256B-handle.png")


def setRegion(reg):
    global regions
    if reg == ONEregionsNAME:
        regions = (ONEregionsNAME, ONEregions)
    elif reg == EUregionsNAME:
        regions = (EUregionsNAME, EUregions)
    elif reg == ALLregionsNAME:
        regions = (ALLregionsNAME, ALLregions)
    else:
        # default
        regions = (ONEregionsNAME, ONEregions)


parser = argparse.ArgumentParser(description='X-HotStuff evaluation')
parser.add_argument("--file",      help="file to plot", type=str, default="")
parser.add_argument("--conf",       type=int, default=0)     # generate a configuration file for 'n' nodes
parser.add_argument("--tvl",        action="store_true")     # throughput vs. latency experiments
parser.add_argument("--tvlaws",     action="store_true")     # throughput vs. latency experiments on AWS
parser.add_argument("--launch",     type=int, default=0)     # launch EC2 instances
parser.add_argument("--aws",        action="store_true")     # run AWS
parser.add_argument("--cluster",    action="store_true")     # run cluster
parser.add_argument("--prepare",    action="store_true")     # prepare cluster
parser.add_argument("--containers", type=int, default=0)     # launch Docker instances
parser.add_argument("--awstest",    action="store_true")     # test AWS
parser.add_argument("--stop",       action="store_true")     # to terminate all instances in the current region
parser.add_argument("--stopall",    action="store_true")     # to terminate all instances in all regions
parser.add_argument("--latest",     action="store_true")     # copies latest experiments to paper
parser.add_argument("--copy",       type=str, default="",  help="copies all files to the AWS address provided as argument")
parser.add_argument("--docker",     action="store_true",   help="runs nodes locally in Docker containers")
parser.add_argument("--repeats",    type=int, default=0,   help="number of repeats per experiment")
parser.add_argument("--repeats2",   type=int, default=0,   help="number of repeats per experiment (2nd level, i.e., regenerates AWS instances)")
parser.add_argument("--faults",     type=str, default="",  help="the number of faults to test, separated by commas: 1,2,3,etc.")
parser.add_argument("--test",       action="store_true",   help="to stop after checking the arguments")
parser.add_argument("--payload",    type=int, default=0,   help="size of payloads in Bytes")
parser.add_argument("--p1",         action="store_true",   help="sets runBase to True (base protocol, i.e., HotStuff)")
parser.add_argument("--p2",         action="store_true",   help="sets runCheap to True (Damysus-C)")
parser.add_argument("--p3",         action="store_true",   help="sets runQuick to True (Damysus-A)")
parser.add_argument("--p4",         action="store_true",   help="sets runComb to True (Damysus)")
parser.add_argument("--p5",         action="store_true",   help="sets runChBase to True (chained base protocol, i.e., chained HotStuff")
parser.add_argument("--p6",         action="store_true",   help="sets runChComb to True (chained Damysus)")
parser.add_argument("--p7",         action="store_true",   help="sets runFree to True (hash&signature-free Damysus)")
parser.add_argument("--p8",         action="store_true",   help="sets runOnep to True (1+1/2 phase Damysus)")
parser.add_argument("--pall",       action="store_true",   help="sets all runXXX to True, i.e., all protocols will be executed")
parser.add_argument("--netlat",     type=int, default=0,   help="network latency in ms")
parser.add_argument("--netvar",     type=int, default=0,   help="variation of the network latency in ms")
parser.add_argument("--clients1",   type=int, default=0,   help="number of clients for the non-chained versions")
parser.add_argument("--clients2",   type=int, default=0,   help="number of clients for the chained versions")
parser.add_argument("--onecore",    action="store_true",   help="sets useMultiCores to False, i.e., use 1 core only to compile")
parser.add_argument("--hw",         action="store_true",   help="sets sgxmode to HW, i.e., sgx will be used in hardware mode")
parser.add_argument("--memory",     type=int, default=0,   help="memory used by docker containers")
parser.add_argument("--cpus",       type=float, default=0, help="cpus used by docker containers")
parser.add_argument("--nocompil",   action="store_true",   help="to not recompile the code at the beginnong of each experiment (should only be used when runnning an already compiled experiment)")
parser.add_argument("--cutoff",     type=int, default=0,   help="time after which the experiments are stopped")
parser.add_argument("--views",      type=int, default=0,   help="number of views to run per experiments")
parser.add_argument("--regions",    type=str, default="",  help="the AWS regions to use (one, eu, all)")
parser.add_argument("--handle",     action="store_true",   help="to plot handling times")
parser.add_argument("--handleonly", action="store_true",   help="to plot handling times only")
parser.add_argument("--crypto",     action="store_true",   help="to plot crypto times")
parser.add_argument("--cryptoonly", action="store_true",   help="to plot crypto times only")
parser.add_argument("--debug",      type=int, default=1,   help="to print debugging information while plotting (0 means no)")
parser.add_argument("--latency",    type=int, default=1,   help="to not print debugging information while plotting (0 means no)")
parser.add_argument("--throughput", type=int, default=1,   help="to not print debugging information while plotting (0 means no)")
args = parser.parse_args()


if args.regions:
    if args.regions in [ONEregionsNAME,EUregionsNAME,ALLregionsNAME]:
        setRegion(args.regions)
        print("SUCCESSFULLY PARSED ARGUMENT - regions is", args.regions)
    else:
        print("UNSUCCESSFULLY PARSED regions ARGUMENT")


if args.views > 0:
    numViews = args.views
    print("SUCCESSFULLY PARSED ARGUMENT - the number of views is now:", numViews)


if args.cutoff > 0:
    cutOffBound = args.cutoff
    print("SUCCESSFULLY PARSED ARGUMENT - the cutoff bound is now:", cutOffBound)


if args.repeats > 0:
    repeats = args.repeats
    print("SUCCESSFULLY PARSED ARGUMENT - the number of repeats is now:", repeats)


if args.repeats2 > 0:
    repeatsL2 = args.repeats2
    print("SUCCESSFULLY PARSED ARGUMENT - the number of 2nd level repeats is now:", repeatsL2)


if args.memory > 0:
    dockerMem = args.memory
    print("SUCCESSFULLY PARSED ARGUMENT - the memory used by docker containers is now (in MB):", dockerMem)


if args.cpus > 0:
    dockerCpu = args.cpus
    print("SUCCESSFULLY PARSED ARGUMENT - the cpus used by docker containers is now:", dockerCpu)


if args.netlat >= 0:
    networkLat = args.netlat
    print("SUCCESSFULLY PARSED ARGUMENT - the network latency (in ms) will be changed using netem to:", networkLat)


if args.netvar >= 0:
    networkVar = args.netvar
    print("SUCCESSFULLY PARSED ARGUMENT - the variation of the network latency (in ms) will be changed using netem to:", networkVar)


if args.payload >= 0:
    payloadSize = args.payload
    print("SUCCESSFULLY PARSED ARGUMENT - the payload size will be:", payloadSize)


if args.docker:
    runDocker = True
    print("SUCCESSFULLY PARSED ARGUMENT - running nodes in Docker containers")


if args.nocompil:
    recompile = False
    print("SUCCESSFULLY PARSED ARGUMENT - will not re-compile the code")


if args.handle:
    plotHandle = True
    print("SUCCESSFULLY PARSED ARGUMENT - will plot handling time")


if args.crypto:
    plotCrypto = True
    print("SUCCESSFULLY PARSED ARGUMENT - will plot crypto time")


if args.handleonly:
    plotHandle = True
    plotView   = False
    print("SUCCESSFULLY PARSED ARGUMENT - will plot handling time only")


if args.cryptoonly:
    plotCrypto = True
    plotView   = False
    print("SUCCESSFULLY PARSED ARGUMENT - will plot crypto time only")


if 0 <= args.debug:
    if args.debug == 0:
        debugPlot = False
        print("SUCCESSFULLY PARSED ARGUMENT - will not print debugging info while plotting")
    else:
        debugPlot = True
        print("SUCCESSFULLY PARSED ARGUMENT - will print debugging info while plotting")


if 0 <= args.latency:
    if args.latency == 0:
        plotLatency = False
        print("SUCCESSFULLY PARSED ARGUMENT - will not plot latency")
    else:
        plotLatency = True
        print("SUCCESSFULLY PARSED ARGUMENT - will plot latency")


if 0 <= args.throughput:
    if args.throughput == 0:
        plotThroughput = False
        print("SUCCESSFULLY PARSED ARGUMENT - will not plot throughput")
    else:
        plotThroughput = True
        print("SUCCESSFULLY PARSED ARGUMENT - will plot throughput")


if args.onecore:
    useMultiCores = False
    print("SUCCESSFULLY PARSED ARGUMENT - will use 1 core only to compare")


if args.hw:
    sgxmode = "HW"
    print("SUCCESSFULLY PARSED ARGUMENT - SGX will be used in hardware mode")


if args.faults:
    l = list(map(lambda x: int(x), args.faults.split(",")))
    faults = l
    print("SUCCESSFULLY PARSED ARGUMENT - we will be testing for f in", l)


if args.p1:
    runBase = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing base protocol")

if args.p2:
    runCheap = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing Damysus-C")

if args.p3:
    runQuick = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing Damysus-A")

if args.p4:
    runComb = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing Damysus")

if args.p5:
    runChBase = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing chained base protocol")

if args.p6:
    runChComb = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing chained Damysus")

if args.p7:
    runFree = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing hash&signature-free Damysus")

if args.p8:
    runOnep = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing 1+1/2 phase Damysus")

if args.pall:
    runBase   = True
    runCheap  = True
    runQuick  = True
    runComb   = True
    runFree   = True
    runOnep   = True
    runChBase = True
    runChComb = True
    print("SUCCESSFULLY PARSED ARGUMENT - testing all protocols")


if args.clients1 > 0:
    numNonChCls = args.clients1
    print("SUCCESSFULLY PARSED ARGUMENT - the number of clients for the non-chained version is now:", numNonChCls)


if args.clients2 > 0:
    numChCls = args.clients2
    print("SUCCESSFULLY PARSED ARGUMENT - the number of clients for the chained version is now:", numChCls)


if args.test:
    print("done")
elif args.file.startswith(statsdir+"/points-") or args.file.startswith(statsdir+"/final-points-"):
    createPlot(args.file)
elif args.file.startswith(statsdir+"/clients-"):
    createTVLplot(args.file,-1)
elif args.conf > 0:
    genLocalConf(args.conf,addresses)
elif args.tvl:
    print("Throughput vs. Latency")
    TVL()
elif args.tvlaws:
    print("Throughput vs. Latency")
    TVLaws()
elif args.launch:
    print("lauching AWS instances")
    startInstances(args.launch,0)
elif args.containers:
    print("lauching Docker containers")
    numContainers = args.containers
    startContainers(numContainers,0)
    prot = Protocol.ONEP
    fact = 2
    if args.p1:
        prop = Protocol.BASE
        fact = 3
    elif args.p2:
        prop = Protocol.CHEAP
        fact = 2
    elif args.p3:
        prop = Protocol.QUICK
        fact = 2
    elif args.p4:
        prop = Protocol.COMB
        fact = 2
    elif args.p5:
        prop = Protocol.CHBASE
        fact = 3
    elif args.p6:
        prop = Protocol.CHCOMB
        fact = 2
    elif args.p7:
        prop = Protocol.FREE
        fact = 2
    elif args.p8:
        prop = Protocol.ONEP
        fact = 2
    else:
        prop = Protocol.ONEP
        fact = 2
    mkParams(protocol=prot,constFactor=fact,numFaults=1,numTrans=400,payloadSize=0)
    for i in range(numContainers):
        instance = dockerBase + str(i)
        src = "Makefile"
        dst = instance + ":/app/"
        subprocess.run([docker + " cp " + src + " " + dst], shell=True, check=True)
        src =  "App/."
        dst = instance + ":/app/App/"
        subprocess.run([docker + " cp " + src + " " + dst], shell=True, check=True)
        src =  "Enclave/."
        dst = instance + ":/app/Enclave/"
        subprocess.run([docker + " cp " + src + " " + dst], shell=True, check=True)
        subprocess.run([docker + " exec -t " + instance + " bash -c \"" + srcsgx + "; make clean; make -j " + str(1) + " SGX_MODE=" + sgxmode + "\""], shell=True, check=True)
elif args.copy:
    print("copying files to AWS instance")
    copyToAddr(args.copy)
elif args.awstest:
    print("AWS")
    testAWS()
elif args.aws:
    print("lauching AWS experiment")
    runAWS()
elif args.cluster:
    print("lauching cluster experiment")
    runCluster()
elif args.prepare:
    print("preparing cluster")
    prepareCluster()
elif args.stop:
    print("terminate all AWS instances in the current region")
    terminateAllInstances()
elif args.stopall:
    print("terminate all AWS instances in all regions")
    terminateAllInstancesAllRegs()
elif args.latest:
    print("copies latest experiments to paper")
    copyLatestExperiments()
else:
    print("Throughput and Latency")
    runExperiments()


#### Run the experiments to compute throughputs & latencies
#runExperiments()

## Debug
#tup = computeAvgStats(recompile=False,protocol=Protocol.COMB,constFactor=2,numFaults=1,numRepeats=1)
#print(tup)
#createPlot(plotFile)
#createPlot("points-01-Apr-2021-15:48:12.821672")
#createPlot("stats/points-08-Apr-2021-15:57:31.873203")
#mkApp(protocol=Protocol.CHEAP,constFactor=2,numFaults=1,numTrans=numTrans,payloadSize=payloadSize)
#createPlot("stats/points-13-Apr-2021-10:18:28.616683")
#createPlot("stats/points-14-Apr-2021-10:58:41.589782")
#createPlot("stats/points-15-Apr-2021-01:10:01.920476")

#### Run the experiments to compute throughput vs. latency
#TVL()

## Debug
#createTVLplot("stats/clients-10-Apr-2021-00:44:17.638744")
#createTVLplot("stats/clients-15-Apr-2021-02:40:47.929625")
