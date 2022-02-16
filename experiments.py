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
runChBase    = False #True
runChComb    = False #True
# Debug versions
runQuickDbg  = False #True
runChCombDbg = False #True
#
plotHandle   = False  # to plot the numbers of just handling messages, without the network
debugPlot    = False  # to print debug info when plotting
showTitle    = True   # to print the title of the figure
expmode      = "" # "TVL"
showLegend1  = True
showLegend2  = False
plotBasic    = True
plotChained  = True
displayPlot  = True # to display a plot once it is generated
showYlabel   = True
displayApp   = "shotwell"

# To set some plotting parameters for specific experiments
whichExp = ""

# For some experiments we start with f nodes dead
deadNodes    = False #True
# if deadNodes then we go with less views and give ourselves more time
if deadNodes:
    numViews = numViews // timeout
    cutOffBound = cutOffBound * 2

# For some experiments we remove the outliers
quantileSize = 10

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
baseHS   = "basic HotStuff"
cheapHS  = "Damysus-C"
quickHS  = "Damysus-A"
combHS   = "Damysus"
baseChHS = "chained HotStuff"
combChHS = "Chained-Damysus"

# Markers
baseMRK   = "P"
cheapMRK  = "o"
quickMRK  = "*"
combMRK   = "X"
baseChMRK = "d"
combChMRK = ">"

# Line styles
baseLS   = ":"
cheapLS  = "--"
quickLS  = "-."
combLS   = "-"
baseChLS = ":"
combChLS = "-"

# Markers
baseCOL   = "black"
cheapCOL  = "blue"
quickCOL  = "green"
combCOL   = "red"
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
WregionsNAME = "Wregions"
Wregions = [(region_USEAST2,  imageID_USEAST2,  secGroup_USEAST2),
            (region_APSEAST2, imageID_APSEAST2, secGroup_APSEAST2),
            (region_EUWEST2,  imageID_EUWEST2,  secGroup_EUWEST2),
            (region_CACENT1,  imageID_CACENT1,  secGroup_CACENT1)]

# US regions
USregionsNAME = "USregions"
USregions = [(region_USEAST1, imageID_USEAST1, secGroup_USEAST1),
             (region_USEAST2, imageID_USEAST2, secGroup_USEAST2),
             (region_USWEST1, imageID_USWEST1, secGroup_USWEST1),
             (region_USWEST2, imageID_USWEST2, secGroup_USWEST2)]

# EU regions
EUregionsNAME = "EUregions"
EUregions = [(region_EUWEST1, imageID_EUWEST1, secGroup_EUWEST1),
             (region_EUWEST2, imageID_EUWEST2, secGroup_EUWEST2),
             (region_EUWEST3, imageID_EUWEST3, secGroup_EUWEST3),
             (region_EUCENT1, imageID_EUCENT1, secGroup_EUCENT1)]
# One region
ONEregionsNAME = "ONEregions"
ONEregions = [(region_USEAST2, imageID_USEAST2, secGroup_USEAST2)]

# All regions
ALLregionsNAME = "ALLregions"
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
ALL2regionsNAME = "ALL2regions"
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

#regions = (USregionsNAME, USregions)
#regions = (EUregionsNAME, EUregions)
#regions = (WregionsNAME, Wregions)
#regions = (ONEregionsNAME, ONEregions)
regions = (ALLregionsNAME, ALLregions)
#regions = (ALL2regionsNAME, ALL2regions)

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


## Code

class Protocol(Enum):
    BASE      = "BASIC_BASELINE"   # basic baseline
    CHEAP     = "BASIC_CHEAP"  # Checker only
    QUICK     = "BASIC_QUICK"  # Accumulator only
    COMB      = "BASIC_CHEAP_AND_QUICK"    # Damysus (Checker + Accumulator)
    CHBASE    = "CHAINED_BASELINE" # chained baseline
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
        subprocess.run(s1 + "App/*.h"       + s2 + "App/",     shell=True, check=True)
        subprocess.run(s1 + "App/*.cpp"     + s2 + "App/",     shell=True, check=True)
        subprocess.run(s1 + "Enclave/*.h"   + s2 + "Enclave/", shell=True, check=True)
        subprocess.run(s1 + "Enclave/*.cpp" + s2 + "Enclave/", shell=True, check=True)
        subprocess.run(s1 + "Enclave/*.edl" + s2 + "Enclave/", shell=True, check=True)
    else:
        subprocess.run(["scp","-i",pem,"-o",sshOpt1,params,sshAdr+":/home/ubuntu/xhotstuff/App/"])


def makeInstances(instanceIds,protocol):
    ncores = 1
    if useMultiCores:
        ncores = numMakeCores
    print(">> making",str(len(instanceIds)),"instance(s) using",str(ncores),"core(s)")

    procs  = []
    make0  = "make -j "+str(ncores)
    make   = make0 + " SGX_MODE="+sgxmode if needsSGX(protocol) else make0 + " server client"

    for (n,i,priv,pub,dns,region) in instanceIds:
        sshAdr = "ubuntu@" + dns
        copyToAddr(sshAdr)
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
        srun   = server + " " + str(n) + " " + str(numFaults) + " " + str(constFactor) + " " + str(numViews) + " " + str(newtimeout)
        cmd    = "\"\"" + srcsgx + " && cd xhotstuff && rm -f stats/* && " + srun + "\"\""
        p      = Popen(["ssh","-i",pem,"-o",sshOpt1,"-ntt",sshAdr,cmd])
        print("the commandline is {}".format(p.args))
        procsRep.append(("R",n,i,priv,pub,dns,region,p))

    print("started", len(procsRep), "replicas")

    # we give some time for the replicas to connect before starting the clients
    wait = 5 + int(math.ceil(math.log(numFaults,2)))
    time.sleep(wait)

    for (n,i,priv,pub,dns,region) in instanceClIds:
        sshAdr = "ubuntu@" + dns
        crun   = client + " " + str(n) + " " + str(numFaults) + " " + str(constFactor) + " " + str(numClTrans) + " " + str(sleepTime) + " " + str(instance)
        cmd    = "\"\"" + srcsgx + " && cd xhotstuff && rm -f stats/* && " + crun + "\"\""
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
        for (tag,n,i,priv,pub,dns,region,p) in procsRep + procsCl:
            # We stop the execution if it takes too long (cutOffBound)
            while (p.poll() is None) and totalTime < cutOffBound:
                time.sleep(1)
                totalTime += 1
            n += 1
            print("processes stopped:", n, "/", len(procsRep + procsCl), "-", p.args)

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
        for (tag,n,i,priv,pub,dns,region,p) in procsRep + procsCl:
            # we print the nodes that haven't finished yet
            if (p.poll() is None):
                print("still running:",(tag,n,i,priv,pub,dns,region,p.poll()))
            p.kill()
        print("------ stopped run ------")
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


def terminateAllInstances():
    for (region,imageID,secGroup) in regions[1]:
        f = open(instFile,'w')
        subprocess.run(["aws","ec2","describe-instances","--region",region,"--filters","Name=image-id,Values="+imageID], stdout=f)
        f.close()

        f = open(instFile,'r')
        instances = json.load(f)
        #print(instances)
        f.close()
        for res in instances["Reservations"]:
            for inst in res["Instances"]:
                i = inst["InstanceId"]
                print(i)
                terminateInstance(region,i)
# End of terminateAllInstances


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


def executeAWS(protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults):
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

    # starts the instances & make them
    (instanceRepIds, instanceClIds) = startInstances(numReps,numClients)
    mkParams(protocol,constFactor,numFaults,numTrans,payloadSize)
    #time.sleep(5)
    makeInstances(instanceRepIds+instanceClIds,protocol)

    for instance in range(repeats):
        clearStatsDir()
        # execute the experiment
        executeInstances(instanceRepIds,instanceClIds,protocol,constFactor,numClTrans,sleepTime,numViews,cutOffBound,numFaults,instance)
        # copy the stats over
        for (n,i,priv,pub,dns,region) in instanceRepIds:
            sshAdr = "ubuntu@" + dns
            subprocess.run(["scp","-i",pem,"-o",sshOpt1,sshAdr+":/home/ubuntu/xhotstuff/stats/*","stats/"])
        (throughputView,latencyView,throughputHandle,latencyHandle) = computeStats(protocol,numFaults,instance,repeats)

    # terminates the instances
    terminateInstances(instanceRepIds + instanceClIds)
# End of executeAWS


def runAWS():
    global numMakeCores
    numMakeCores = 1

    for numFaults in faults:
        # ------
        # HotStuff-like baseline
        if runBase:
            executeAWS(protocol=Protocol.BASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Cheap-HotStuff (TEE locked/prepared blocks)
        if runCheap:
            executeAWS(protocol=Protocol.CHEAP,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Quick-HotStuff (Accumulator)
        if runQuick:
            executeAWS(protocol=Protocol.QUICK,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Quick-HotStuff (Accumulator) - debug version
        if runQuickDbg:
            executeAWS(protocol=Protocol.QUICKDBG,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Combines Cheap&Quick-HotStuff
        if runComb:
            executeAWS(protocol=Protocol.COMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Chained HotStuff-like baseline
        if runChBase:
            executeAWS(protocol=Protocol.CHBASE,constFactor=3,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Chained Cheap&Quick
        if runChComb:
            executeAWS(protocol=Protocol.CHCOMB,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # Chained Cheap&Quick - debug version
        if runChCombDbg:
            executeAWS(protocol=Protocol.CHCOMBDBG,constFactor=2,numClTrans=numClTrans,sleepTime=sleepTime,numViews=numViews,cutOffBound=cutOffBound,numFaults=numFaults)
        # ------
        # We now terminate all instances just in case
        #terminateAllInstances()

    print("num complete runs=", completeRuns)
    print("num aborted runs=", abortedRuns)
    print("aborted runs:", aborted)

    createPlot(pointsFile)
# End of runAWS


## Returns True if the protocol requires SGX
def needsSGX(protocol):
    if (protocol == Protocol.BASE or protocol == Protocol.CHBASE or protocol == Protocol.QUICKDBG or protocol == Protocol.CHCOMBDBG):
        return False
    else:
        return True
# End of needsSGX


def clearStatsDir():
    # Removing all (temporary) files in stats dir
    files1 = glob.glob(statsdir+"/throughput-view*")
    files2 = glob.glob(statsdir+"/throughput-handle*")
    files3 = glob.glob(statsdir+"/latency-view*")
    files4 = glob.glob(statsdir+"/latency-handle*")
    files5 = glob.glob(statsdir+"/done*")
    files6 = glob.glob(statsdir+"/client-throughput-latency*")
    for f in files1 + files2 + files3 + files4 + files5 + files6:
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
            instance = dockerBase + i
            dst = instance + ":/app/App/"
            subprocess.run(["docker cp " + params + " " + dst], shell=True, check=True)
            # DEBUG begin
            #subprocess.run(["docker cp App/Nodes.cpp " + instance + ":/app/App/"], shell=True, check=True)
            # DEBUG end
            subprocess.run(["docker exec -t " + instance + " bash -c \"make clean\""], shell=True, check=True)
            if needsSGX(protocol):
                subprocess.run(["docker exec -t " + instance + " bash -c \"" + srcsgx + "; make -j " + str(ncores) + " SGX_MODE=" + sgxmode + "\""], shell=True, check=True)
            else:
                subprocess.run(["docker exec -t " + instance + " bash -c \"make -j " + str(ncores) + " server client\""], shell=True, check=True)
    else:
        subprocess.call(["make","clean"])
        if needsSGX(protocol):
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
        remaining = subsClients
        numTotClients = len(subsClients)
        while 0 < len(remaining) and totalTime < cutOffBound:
            print(str(len(remaining)) + " remaining clients out of " + str(numTotClients) + ":", remaining)
            cFileBase = "client-throughput-latency-" + str(instance)
            if runDocker:
                rem = remaining
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
        remaining = subsReps
        # We wait here for all processes to complete
        # but we stop the execution if it takes too long (cutOffBound)
        while 0 < len(remaining) and totalTime < cutOffBound:
            print("remaining processes:", remaining)
            # We filter out the ones that are done. x is of the form (t,i,p)
            if runDocker:
                rem = remaining
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


def computeStats(protocol,numFaults,instance,repeats):
    # Computing throughput and latency
    throughputViewVal=0.0
    throughputViewNum=0
    latencyViewVal=0.0
    latencyViewNum=0

    throughputHandleVal=0.0
    throughputHandleNum=0
    latencyHandleVal=0.0
    latencyHandleNum=0

    printNodePointComment(protocol,numFaults,instance,repeats)

    files = glob.glob(statsdir+"/*")
    for filename in files:
        #print("reading", filename)
        #print("read:", s, "from", filename)
        # total throughput including network
        if filename.startswith(statsdir+"/throughput-view"):
            f = open(filename, "r")
            s = f.read()
            val = float(s)
            throughputViewNum += 1
            throughputViewVal += val
            printNodePoint(protocol,numFaults,"throughput-view",val)
        # total latency including network
        elif filename.startswith(statsdir+"/latency-view"):
            f = open(filename, "r")
            s = f.read()
            val = float(s)
            latencyViewNum += 1
            latencyViewVal += val
            printNodePoint(protocol,numFaults,"latency-view",val)
        # throughput execluding network
        if filename.startswith(statsdir+"/throughput-handle"):
            f = open(filename, "r")
            s = f.read()
            val = float(s)
            throughputHandleNum += 1
            throughputHandleVal += val
            printNodePoint(protocol,numFaults,"throughput-handle",val)
        # latency excluding network
        elif filename.startswith(statsdir+"/latency-handle"):
            f = open(filename, "r")
            s = f.read()
            val = float(s)
            latencyHandleNum += 1
            latencyHandleVal += float(s)
            printNodePoint(protocol,numFaults,"latency-handle",val)

    throughputView = throughputViewVal/throughputViewNum if throughputViewNum > 0 else 0.0
    latencyView    = latencyViewVal/latencyViewNum       if latencyViewNum > 0    else 0.0

    throughputHandle = throughputHandleVal/throughputHandleNum if throughputHandleNum > 0 else 0.0
    latencyHandle    = latencyHandleVal/latencyHandleNum       if latencyHandleNum > 0    else 0.0

    print("throughput-view:",   throughputView,   "out of", throughputViewNum)
    print("latency-view:",      latencyView,      "out of", latencyViewNum)
    print("throughput-handle:", throughputHandle, "out of", throughputHandleNum)
    print("latency-handle:",    latencyHandle,    "out of", latencyHandleNum)

    return (throughputView, latencyView, throughputHandle, latencyHandle)
## End of computeStats


def startContainers(numReps,numClients):
    print("running in docker mode, starting" , numReps, "containers for the replicas and", numClients, "for the clients")

    global ipsOfNodes

    lr = list(map(lambda x: (True, str(x)), list(range(numReps))))            # replicas
    lc = list(map(lambda x: (False, "c" + str(x)), list(range(numClients))))  # clients
    lall = lr + lc

    for (isRep, i) in lall:
        instance = dockerBase + i
        # We stop and remove the Doker instance if it is still exists
        subprocess.run([docker + " stop " + instance], shell=True) #, check=True)
        subprocess.run([docker + " rm " + instance], shell=True) #, check=True)
        # TODO: make sure to cover all the ports
        opt1 = "--expose=8000-9999"
        opt2 = "--network=\"bridge\""
        opt3 = "--cap-add=NET_ADMIN"
        opt4 = "--name " + instance
        opts = " ".join([opt1, opt2, opt3, opt4])
        # We start the Docker instance
        subprocess.run([docker + " run -td " + opts + " " + dockerBase], shell=True, check=True)
        subprocess.run([docker + " exec -t " + instance + " bash -c \"" + srcsgx + "; mkdir " + statsdir + "\""], shell=True, check=True)
        # Set the network latency
        if 0 < networkLat:
            print("----changing network latency to " + str(networkLat) + "ms")
            latcmd = "tc qdisc add dev eth0 root netem delay " + str(networkLat) + "ms"
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
        subprocess.run([docker + " stop " + instance], shell=True, check=True)
        subprocess.run([docker + " rm " + instance], shell=True, check=True)
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
    throughputHandles=[]
    latencyHandles=[]

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
        (throughputView,latencyView,throughputHandle,latencyHandle) = computeStats(protocol,numFaults,i,numRepeats)
        if throughputView > 0 and latencyView > 0 and throughputHandle > 0 and latencyHandle > 0:
            throughputViews.append(throughputView)
            latencyViews.append(latencyView)
            throughputHandles.append(throughputHandle)
            latencyHandles.append(latencyHandle)
            goodValues += 1

    if runDocker:
        stopContainers(numReps,numClients)

    throughputView   = sum(throughputViews)/goodValues   if goodValues > 0 else 0.0
    latencyView      = sum(latencyViews)/goodValues      if goodValues > 0 else 0.0
    throughputHandle = sum(throughputHandles)/goodValues if goodValues > 0 else 0.0
    latencyHandle    = sum(latencyHandles)/goodValues    if goodValues > 0 else 0.0

    print("avg throughput (view):",   throughputView)
    print("avg latency (view):",      latencyView)
    print("avg throughput (handle):", throughputHandle)
    print("avg latency (handle):",    latencyHandle)

    return (throughputView, latencyView, throughputHandle, latencyHandle)
# End of computeAvgStats


def dict2lists(d):
    faults = []
    vals   = []
    nums   = []

    # We create the lists of points from the dictionaries
    for f,(val,num) in d.items():
        faults.append(f)
        v = val/num if num > 0 else 0.0
        vals.append(v)
        nums.append(num)

    return (faults,vals,nums)
# End of dict2lists



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

    print(nameNew + "(#faults/value): " + str(newLst))
    print(nameNew + "(avg/min/ax): " + "avg=" + str(newAvg) + ";min=" + str(newMin) + ";max=" + str(newMax))
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
    dictTVChBase = {}
    dictTVChComb = {}
    # throughput-handle
    dictTHBase   = {}
    dictTHCheap  = {}
    dictTHQuick  = {}
    dictTHComb   = {}
    dictTHChBase = {}
    dictTHChComb = {}
    # latency-view
    dictLVBase   = {}
    dictLVCheap  = {}
    dictLVQuick  = {}
    dictLVComb   = {}
    dictLVChBase = {}
    dictLVChComb = {}
    # latency-handle
    dictLHBase   = {}
    dictLHCheap  = {}
    dictLHQuick  = {}
    dictLHComb   = {}
    dictLHChBase = {}
    dictLHChComb = {}

    # We accumulate all the points in dictionaries
    print("reading points from:", pFile)
    f = open(pFile,'r')
    for line in f.readlines():
        if not line.startswith("#"):
            [prot,faults,point]   = line.split(" ")
            [protTag,protVal]     = prot.split("=")
            [faultsTag,faultsVal] = faults.split("=")
            [pointTag,pointVal]   = point.split("=")
            numFaults=int(faultsVal)
            if float(pointVal) < float('inf'):
                # Throughputs-view
                if pointTag == "throughput-view" and protVal == "BASIC_BASELINE":
                    (val,num) = dictTVBase.get(numFaults,(0.0,0))
                    dictTVBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_CHEAP":
                    (val,num) = dictTVCheap.get(numFaults,(0.0,0))
                    dictTVCheap.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_QUICK":
                    (val,num) = dictTVQuick.get(numFaults,(0.0,0))
                    dictTVQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictTVQuick.get(numFaults,(0.0,0))
                    dictTVQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictTVComb.get(numFaults,(0.0,0))
                    dictTVComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictTVChBase.get(numFaults,(0.0,0))
                    dictTVChBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictTVChComb.get(numFaults,(0.0,0))
                    dictTVChComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-view" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictTVChComb.get(numFaults,(0.0,0))
                    dictTVChComb.update({numFaults:(val+float(pointVal),num+1)})
                # Throughputs-handle
                if pointTag == "throughput-handle" and protVal == "BASIC_BASELINE":
                    (val,num) = dictTHBase.get(numFaults,(0.0,0))
                    dictTHBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "BASIC_CHEAP":
                    (val,num) = dictTHCheap.get(numFaults,(0.0,0))
                    dictTHCheap.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "BASIC_QUICK":
                    (val,num) = dictTHQuick.get(numFaults,(0.0,0))
                    dictTHQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictTHQuick.get(numFaults,(0.0,0))
                    dictTHQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictTHComb.get(numFaults,(0.0,0))
                    dictTHComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictTHChBase.get(numFaults,(0.0,0))
                    dictTHChBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictTHChComb.get(numFaults,(0.0,0))
                    dictTHChComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "throughput-handle" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictTHChComb.get(numFaults,(0.0,0))
                    dictTHChComb.update({numFaults:(val+float(pointVal),num+1)})
                # Latencies-view
                if pointTag == "latency-view" and protVal == "BASIC_BASELINE":
                    (val,num) = dictLVBase.get(numFaults,(0.0,0))
                    dictLVBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_CHEAP":
                    (val,num) = dictLVCheap.get(numFaults,(0.0,0))
                    dictLVCheap.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_QUICK":
                    (val,num) = dictLVQuick.get(numFaults,(0.0,0))
                    dictLVQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictLVQuick.get(numFaults,(0.0,0))
                    dictLVQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictLVComb.get(numFaults,(0.0,0))
                    dictLVComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictLVChBase.get(numFaults,(0.0,0))
                    dictLVChBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictLVChComb.get(numFaults,(0.0,0))
                    dictLVChComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-view" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictLVChComb.get(numFaults,(0.0,0))
                    dictLVChComb.update({numFaults:(val+float(pointVal),num+1)})
                # Latencies-handle
                if pointTag == "latency-handle" and protVal == "BASIC_BASELINE":
                    (val,num) = dictLHBase.get(numFaults,(0.0,0))
                    dictLHBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "BASIC_CHEAP":
                    (val,num) = dictLHCheap.get(numFaults,(0.0,0))
                    dictLHCheap.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "BASIC_QUICK":
                    (val,num) = dictLHQuick.get(numFaults,(0.0,0))
                    dictLHQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "BASIC_QUICK_DEBUG":
                    (val,num) = dictLHQuick.get(numFaults,(0.0,0))
                    dictLHQuick.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "BASIC_CHEAP_AND_QUICK":
                    (val,num) = dictLHComb.get(numFaults,(0.0,0))
                    dictLHComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "CHAINED_BASELINE":
                    (val,num) = dictLHChBase.get(numFaults,(0.0,0))
                    dictLHChBase.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "CHAINED_CHEAP_AND_QUICK":
                    (val,num) = dictLHChComb.get(numFaults,(0.0,0))
                    dictLHChComb.update({numFaults:(val+float(pointVal),num+1)})
                if pointTag == "latency-handle" and protVal == "CHAINED_CHEAP_AND_QUICK_DEBUG":
                    (val,num) = dictLHChComb.get(numFaults,(0.0,0))
                    dictLHChComb.update({numFaults:(val+float(pointVal),num+1)})
    f.close()

    # We convert the dictionaries to lists
    # throughput-view
    (faultsTVBase,   valsTVBase,   numsTVBase)   = dict2lists(dictTVBase)
    (faultsTVCheap,  valsTVCheap,  numsTVCheap)  = dict2lists(dictTVCheap)
    (faultsTVQuick,  valsTVQuick,  numsTVQuick)  = dict2lists(dictTVQuick)
    (faultsTVComb,   valsTVComb,   numsTVComb)   = dict2lists(dictTVComb)
    (faultsTVChBase, valsTVChBase, numsTVChBase) = dict2lists(dictTVChBase)
    (faultsTVChComb, valsTVChComb, numsTVChComb) = dict2lists(dictTVChComb)
    # throughput-handle
    (faultsTHBase,   valsTHBase,   numsTHBase)   = dict2lists(dictTHBase)
    (faultsTHCheap,  valsTHCheap,  numsTHCheap)  = dict2lists(dictTHCheap)
    (faultsTHQuick,  valsTHQuick,  numsTHQuick)  = dict2lists(dictTHQuick)
    (faultsTHComb,   valsTHComb,   numsTHComb)   = dict2lists(dictTHComb)
    (faultsTHChBase, valsTHChBase, numsTHChBase) = dict2lists(dictTHChBase)
    (faultsTHChComb, valsTHChComb, numsTHChComb) = dict2lists(dictTHChComb)
    # latency-view
    (faultsLVBase,   valsLVBase,   numsLVBase)   = dict2lists(dictLVBase)
    (faultsLVCheap,  valsLVCheap,  numsLVCheap)  = dict2lists(dictLVCheap)
    (faultsLVQuick,  valsLVQuick,  numsLVQuick)  = dict2lists(dictLVQuick)
    (faultsLVComb,   valsLVComb,   numsLVComb)   = dict2lists(dictLVComb)
    (faultsLVChBase, valsLVChBase, numsLVChBase) = dict2lists(dictLVChBase)
    (faultsLVChComb, valsLVChComb, numsLVChComb) = dict2lists(dictLVChComb)
    # latency-handle
    (faultsLHBase,   valsLHBase,   numsLHBase)   = dict2lists(dictLHBase)
    (faultsLHCheap,  valsLHCheap,  numsLHCheap)  = dict2lists(dictLHCheap)
    (faultsLHQuick,  valsLHQuick,  numsLHQuick)  = dict2lists(dictLHQuick)
    (faultsLHComb,   valsLHComb,   numsLHComb)   = dict2lists(dictLHComb)
    (faultsLHChBase, valsLHChBase, numsLHChBase) = dict2lists(dictLHChBase)
    (faultsLHChComb, valsLHChComb, numsLHChComb) = dict2lists(dictLHChComb)

    print("throughputs (baseline/cheap/quick/combined/chained-baseline/chained-combined)")
    print((faultsTVBase,   valsTVBase,   numsTVBase))
    print((faultsTVCheap,  valsTVCheap,  numsTVCheap))
    print((faultsTVQuick,  valsTVQuick,  numsTVQuick))
    print((faultsTVComb,   valsTVComb,   numsTVComb))
    print((faultsTVChBase, valsTVChBase, numsTVChBase))
    print((faultsTVChComb, valsTVChComb, numsTVChComb))
    print("latencies (baseline/cheap/quick/combined/chained-baseline/chained-combined)")
    print((faultsLVBase,   valsLVBase,   numsLVBase))
    print((faultsLVCheap,  valsLVCheap,  numsLVCheap))
    print((faultsLVQuick,  valsLVQuick,  numsLVQuick))
    print((faultsLVComb,   valsLVComb,   numsLVComb))
    print((faultsLVChBase, valsLVChBase, numsLVChBase))
    print((faultsLVChComb, valsLVChComb, numsLVChComb))

    print("Throughput gain (basic versions):")
    # non-chained
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,cheapHS,faultsTVCheap,valsTVCheap)
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,quickHS,faultsTVQuick,valsTVQuick)
    getPercentage(False,baseHS,faultsTVBase,valsTVBase,combHS, faultsTVComb, valsTVComb)
    # chained
    getPercentage(False,baseChHS,faultsTVChBase,valsTVChBase,combChHS,faultsTVChComb,valsTVChComb)

    print("Latency gain (basic versions):")
    # non-chained
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,cheapHS,faultsLVCheap,valsLVCheap)
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,quickHS,faultsLVQuick,valsLVQuick)
    getPercentage(True,baseHS,faultsLVBase,valsLVBase,combHS, faultsLVComb, valsLVComb)
    # chained
    getPercentage(True,baseChHS,faultsLVChBase,valsLVChBase,combChHS,faultsLVChComb,valsLVChComb)

    LW = 1 # linewidth
    MS = 5 # markersize
    XYT = (0,5)

    #plt.figure(figsize=(6, 6))#, dpi=80)

    ## Plotting
    print("plotting")
    fig, axs = plt.subplots(2)
    if showTitle:
        if debugPlot:
            fig.suptitle("Throughputs (top) & Latencies (bottom) (file="+pFile+")")
        else:
            fig.suptitle("Throughputs (top) & Latencies (bottom)")

    adjustFigAspect(fig,aspect=.9)

    # naming the x/y axis
    #axs[0].set(xlabel="#faults", ylabel="throughput")
    if showYlabel:
        axs[0].set(ylabel="throughput (Kops/s)")
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
    if plotBasic:
        axs[0].plot(faultsTVBase,   valsTVBase,   color=baseCOL,   linewidth=LW, marker=baseMRK,   markersize=MS, linestyle=baseLS,   label=baseHS)
        axs[0].plot(faultsTVCheap,  valsTVCheap,  color=cheapCOL,  linewidth=LW, marker=cheapMRK,  markersize=MS, linestyle=cheapLS,  label=cheapHS)
        axs[0].plot(faultsTVQuick,  valsTVQuick,  color=quickCOL,  linewidth=LW, marker=quickMRK,  markersize=MS, linestyle=quickLS,  label=quickHS)
        axs[0].plot(faultsTVComb,   valsTVComb,   color=combCOL,   linewidth=LW, marker=combMRK,   markersize=MS, linestyle=combLS,   label=combHS)
    if plotChained:
        axs[0].plot(faultsTVChBase, valsTVChBase, color=baseChCOL, linewidth=LW, marker=baseChMRK, markersize=MS, linestyle=baseChLS, label=baseChHS)
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
        if plotChained:
            for x,y,z in zip(faultsTVChBase, valsTVChBase, numsTVChBase):
                axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(faultsTVChComb, valsTVChComb, numsTVChComb):
                axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
    if plotHandle:
        if plotBasic:
            axs[0].plot(faultsTHBase,   valsTHBase,   color=baseCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=baseLS,   label=baseHS+" (handle)")
            axs[0].plot(faultsTHCheap,  valsTHCheap,  color=cheapCOL,  linewidth=LW, marker="+", markersize=MS, linestyle=cheapLS,  label=cheapHS+" (handle)")
            axs[0].plot(faultsTHQuick,  valsTHQuick,  color=quickCOL,  linewidth=LW, marker="+", markersize=MS, linestyle=quickLS,  label=quickHS+" (handle)")
            axs[0].plot(faultsTHComb,   valsTHComb,   color=combCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=combLS,   label=combHS+" (handle)")
        if plotChained:
            axs[0].plot(faultsTHChBase, valsTHChBase, color=baseChCOL, linewidth=LW, marker="+", markersize=MS, linestyle=baseChLS, label=baseChHS+" (handle)")
            axs[0].plot(faultsTHChComb, valsTHChComb, color=combChCOL, linewidth=LW, marker="+", markersize=MS, linestyle=combChLS, label=combChHS+" (handle)")
        if debugPlot:
            if plotBasic:
                for x,y,z in zip(faultsTHBase, valsTHBase, numsTHBase):
                    axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsTHCheap, valsTHCheap, numsTHCheap):
                    axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsTHQuick, valsTHQuick, numsTHQuick):
                    axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsTHComb, valsTHComb, numsTHComb):
                    axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            if plotChained:
                for x,y,z in zip(faultsTHChBase, valsTHChBase, numsTHChBase):
                    axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsTHChComb, valsTHChComb, numsTHChComb):
                    axs[0].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')

    # legend
    if showLegend1:
        axs[0].legend(ncol=2,prop={'size': 9})

    # naming the x/y axis
    if showYlabel:
        axs[1].set(xlabel="#faults", ylabel="latency (ms)")
    else:
        axs[1].set(xlabel="#faults")
    axs[1].set_yscale('log')
    if whichExp == "EUexp1":
        axs[1].set_yticks((5,100,600))
        axs[1].set_ylim([5,600])
        axs[1].get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    elif whichExp == "ALLexp1":
        axs[1].set_yticks((60,100,1000))
        axs[1].set_ylim([60,1000])
        axs[1].get_yaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
    # giving a title to my graph
    #axs[1].set_title("latencies")
    # plotting the points
    if plotBasic:
        axs[1].plot(faultsLVBase,   valsLVBase,   color=baseCOL,   linewidth=LW, marker=baseMRK,   markersize=MS, linestyle=baseLS,   label=baseHS)
        axs[1].plot(faultsLVCheap,  valsLVCheap,  color=cheapCOL,  linewidth=LW, marker=cheapMRK,  markersize=MS, linestyle=cheapLS,  label=cheapHS)
        axs[1].plot(faultsLVQuick,  valsLVQuick,  color=quickCOL,  linewidth=LW, marker=quickMRK,  markersize=MS, linestyle=quickLS,  label=quickHS)
        axs[1].plot(faultsLVComb,   valsLVComb,   color=combCOL,   linewidth=LW, marker=combMRK,   markersize=MS, linestyle=combLS,   label=combHS)
    if plotChained:
        axs[1].plot(faultsLVChBase, valsLVChBase, color=baseChCOL, linewidth=LW, marker=baseChMRK, markersize=MS, linestyle=baseChLS, label=baseChHS)
        axs[1].plot(faultsLVChComb, valsLVChComb, color=combChCOL, linewidth=LW, marker=combChMRK, markersize=MS, linestyle=combChLS, label=combChHS)
    if debugPlot:
        if plotBasic:
            for x,y,z in zip(faultsLVBase, valsLVBase, numsLVBase):
                axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(faultsLVCheap, valsLVCheap, numsLVCheap):
                axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(faultsLVQuick, valsLVQuick, numsLVQuick):
                axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(faultsLVComb, valsLVComb, numsLVComb):
                axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
        if plotChained:
            for x,y,z in zip(faultsLVChBase, valsLVChBase, numsLVChBase):
                axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            for x,y,z in zip(faultsLVChComb, valsLVChComb, numsLVChComb):
                axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
    if plotHandle:
        if plotBasic:
            axs[1].plot(faultsLHBase,   valsLHBase,   color=baseCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=baseLS,   label=baseHS+" (handle)")
            axs[1].plot(faultsLHCheap,  valsLHCheap,  color=cheapCOL,  linewidth=LW, marker="+", markersize=MS, linestyle=cheapLS,  label=cheapHS+" (handle)")
            axs[1].plot(faultsLHQuick,  valsLHQuick,  color=quickCOL,  linewidth=LW, marker="+", markersize=MS, linestyle=quickLS,  label=quickHS+" (handle)")
            axs[1].plot(faultsLHComb,   valsLHComb,   color=combCOL,   linewidth=LW, marker="+", markersize=MS, linestyle=combLS,   label=combHS+" (handle)")
        if plotChained:
            axs[1].plot(faultsLHChBase, valsLHChBase, color=baseChCOL, linewidth=LW, marker="+", markersize=MS, linestyle=baseChLS, label=baseChHS+" (handle)")
            axs[1].plot(faultsLHChComb, valsLHChComb, color=combChCOL, linewidth=LW, marker="+", markersize=MS, linestyle=combChLS, label=combChHS+" (handle)")
        if debugPlot:
            if plotBasic:
                for x,y,z in zip(faultsLHBase, valsLHBase, numsLHBase):
                    axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsLHCheap, valsLHCheap, numsLHCheap):
                    axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsLHQuick, valsLHQuick, numsLHQuick):
                    axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsLHComb, valsLHComb, numsLHComb):
                    axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
            if plotChained:
                for x,y,z in zip(faultsLHChBase, valsLHChBase, numsLHChBase):
                    axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
                for x,y,z in zip(faultsLHChComb, valsLHChComb, numsLHChComb):
                    axs[1].annotate(z,(x,y),textcoords="offset points",xytext=XYT,ha='center')
    # legend
    if showLegend2:
        axs[1].legend(prop={'size': 7})

    #fig.subplots_adjust(hspace=0.5)
    fig.savefig(plotFile, bbox_inches='tight', pad_inches=0.05)
    print("plot is in", plotFile)
    if displayPlot:
        try:
            subprocess.call([displayApp, plotFile])
        except:
            print("couldn't display the plot using '" + displayApp + "'. Consider changing the 'displayApp' variable.")
# End of createPlot


def runExperiments():
    # Creating stats directory
    Path(statsdir).mkdir(parents=True, exist_ok=True)

    recompile = True

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
    num = int(l/quantileSize)

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
        plt.plot(TBase,   LBase,   color=baseCOL,   linewidth=LW, marker=baseMRK,   markersize=MS, linestyle=baseLS,   label=baseHS)
        plt.plot(TCheap,  LCheap,  color=cheapCOL,  linewidth=LW, marker=cheapMRK,  markersize=MS, linestyle=cheapLS,  label=cheapHS)
        plt.plot(TQuick,  LQuick,  color=quickCOL,  linewidth=LW, marker=quickMRK,  markersize=MS, linestyle=quickLS,  label=quickHS)
        plt.plot(TComb,   LComb,   color=combCOL,   linewidth=LW, marker=combMRK,   markersize=MS, linestyle=combLS,   label=combHS)
    if plotChained:
        plt.plot(TChBase, LChBase, color=baseChCOL, linewidth=LW, marker=baseChMRK, markersize=MS, linestyle=baseChLS, label=baseChHS)
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
    global showYlabel
    global showLegend1
    global showLegend2
    global whichExp
    global showTitle

    showTitle   = False

    plotBasic   = True
    plotChained = True

    showYlabel  = True
    showLegend1 = True
    showLegend2 = False

    # EUregions, payload=256
    whichExp  = "EUexp1"
    pointFile = statsdir+"/points-09-Sep-2021-14:37:34.270859"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-EUregs-256B.png")

    showYlabel  = False
    showLegend1 = False
    showLegend2 = False

    # EUregions, payload=0
    whichExp  = "EUexp1"
    pointFile = statsdir+"/points-18-Sep-2021-08:40:10.075174"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-EUregs-0B.png")

    # ALLregions, payload=256
    whichExp  = "ALLexp1"
    pointFile = statsdir+"/points-12-Sep-2021-21:22:48.294547-v2"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ALLregs-256B.png")

    # ALLregions, payload=0
    whichExp  = "ALLexp1"
    pointFile = statsdir+"/points-23-Sep-2021-20:57:01.200810-v2"
    plotFile  = statsdir + "/plot-" + timestampStr + ".png"
    createPlot(pointFile)
    copyfile(plotFile,"../figures/eval-ALLregs-0B.png")

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



parser = argparse.ArgumentParser(description='X-HotStuff evaluation')
parser.add_argument("--file",     help="file to plot", type=str, default="")
parser.add_argument("--conf",     type=int, default=0)     # generate a configuration file for 'n' nodes
parser.add_argument("--tvl",      action="store_true")     # throughput vs. latency experiments
parser.add_argument("--tvlaws",   action="store_true")     # throughput vs. latency experiments on AWS
parser.add_argument("--launch",   type=int, default=0)     # launch EC2 instances
parser.add_argument("--aws",      action="store_true")     # run AWS
parser.add_argument("--awstest",  action="store_true")     # test AWS
parser.add_argument("--stop",     action="store_true")     # to terminate all instances
parser.add_argument("--latest",   action="store_true")     # copies latest experiments to paper
parser.add_argument("--copy",     type=str, default="")
parser.add_argument("--docker",   action="store_true",  help="runs nodes locally in Docker containers")
parser.add_argument("--repeats",  type=int, default=0,  help="number of repeats per experiment")
parser.add_argument("--faults",   type=str, default="", help="the number of faults to test, separated by commas: 1,2,3,etc.")
parser.add_argument("--test",     action="store_true",  help="to stop after checking the arguments")
parser.add_argument("--payload",  type=int, default=0,  help="size of payloads in Bytes")
parser.add_argument("--p1",       action="store_true",  help="sets runBase to True (base protocol, i.e., HotStuff)")
parser.add_argument("--p2",       action="store_true",  help="sets runCheap to True (Damysus-C)")
parser.add_argument("--p3",       action="store_true",  help="sets runQuick to True (Damysus-A)")
parser.add_argument("--p4",       action="store_true",  help="sets runComb to True (Damysus)")
parser.add_argument("--p5",       action="store_true",  help="sets runChBase to True (chained base protocol, i.e., chained HotStuff")
parser.add_argument("--p6",       action="store_true",  help="sets runChComb to True (chained Damysus)")
parser.add_argument("--pall",     action="store_true",  help="sets all runXXX to True, i.e., all protocols will be executed")
parser.add_argument("--netlat",   type=int, default=0,  help="network latency in ms")
parser.add_argument("--clients1", type=int, default=0,  help="number of clients for the non-chained versions")
parser.add_argument("--clients2", type=int, default=0,  help="number of clients for the chained versions")
parser.add_argument("--onecore",  action="store_true",  help="sets useMultiCores to False, i.e., use 1 core only to compile")
args = parser.parse_args()


if args.repeats > 0:
    repeats = args.repeats
    print("SUCCESSFULLY PARSED ARGUMENT - the number of repeats is now:", repeats)


if args.netlat >= 0:
    networkLat = args.netlat
    print("SUCCESSFULLY PARSED ARGUMENT - the network latency (in ms) will be changed using netem to:", networkLat)


if args.payload >= 0:
    payloadSize = args.payload
    print("SUCCESSFULLY PARSED ARGUMENT - the payload size will be:", payloadSize)


if args.docker:
    runDocker = True
    print("SUCCESSFULLY PARSED ARGUMENT - running nodes in Docker containers")


if args.onecore:
    useMultiCores = False
    print("SUCCESSFULLY PARSED ARGUMENT - will use 1 core only to compare")


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

if args.pall:
    runBase   = True
    runCheap  = True
    runQuick  = True
    runComb   = True
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
elif args.copy:
    print("copying files to AWS instance")
    copyToAddr(args.copy)
elif args.awstest:
    print("AWS")
    testAWS()
elif args.aws:
    print("lauching AWS experiment")
    runAWS()
elif args.stop:
    print("terminate all AWS instances")
    terminateAllInstances()
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
