# Blockchain Application and SGX Trusted Component Documentation

## Table of Contents

1. [Overview](#overview)
2. [Blockchain Application (APP directory)](#blockchain-application)
   - [1. Introduction](#1-introduction)
   - [2. Directory Structure](#2-directory-structure)
   - [3. Components](#3-components)
   - [4. Dependencies](#4-dependencies)
   - [5. Configuration](#5-configuration)
   - [6. Usage](#6-usage)
   - [7. Examples](#7-examples)
   - [8. Tests](#8-tests)
   - [9. Troubleshooting](#9-troubleshooting)

3. [SGX Trusted Component (Enclave directory)](#sgx-trusted-component)
   - [1. Introduction](#1-introduction-1)
   - [2. Directory Structure](#2-directory-structure-1)
   - [3. Components](#3-components-1)
   - [4. Dependencies](#4-dependencies-1)
   - [5. Configuration](#5-configuration-1)
   - [6. Usage](#6-usage-1)
   - [7. Examples](#7-examples-1)
   - [8. Tests](#8-tests-1)
   - [9. Troubleshooting](#9-troubleshooting-1)

## Overview

Create a robust blockchain relying on n=2f+1. This is done by prohibiting equivocation, allowing us to remove a phase out of the Hotstuff protocol and using less replicas than the traditional n=3f+1 configuration, where f is the number of faulty replicas. This protocol prevents equivocation by certifying each message using a trusted component. The replica is described in the APP directory, whereas the trusted component is defined in the Enclave directory. Additionally, there is an experiments.py file that configures the proper replicas and trusted components.

Terms/abbreviations to consider:
Ch: chained
OP: OnePhase
R: round
V: view
Cheap: CHECKER
Quick: ACCUMULATOR
Free: Light-Damysus (no hash and signatures)
TVL: throughput versus latency

Trouble: containers do seem to be created, but in the logging there seems to be no container found when they search for them. 
Solution: jq command for finding IP addresses subprocess.run does not come preinstalled 

Trouble: experiments.py tries to perform a rm on app/stats, which is not an existing directory. 


## Blockchain Application

### 1. Introduction

The Damysus protocol has several phases per view. During each a leader will propose a set of transactions to confirm. This is done in the prepare phase. This is then sent to all other participating replicas. After this, the backups confirm the set of transactions and prepare a response using their trusted component, which they send to the leader. The leader collects responses, and send a combination of these replies to the other replicas. These receive this combination, store this in their trusted component, which concludes the pre-commit phase. This store is send to the leader, which collects and combines these stores to communicate to the other replicas in the decide phase. If the replicas receive a valid combination in this decide phase, they execute the set of transactions and reply to external clients. This concludes the decide phase. After this, the new-view phase starts, where a new leader collects new-view messages from replicas

### 2. Directory Structure

All of the cpp files have a corresponding .h file, except for App.cpp and Client.cpp.
- APP
    - App.cpp:
    - Accumm.cpp: basis Accumulator data structure, including toString
    - Auth.cpp: Authorization data structure
    - Auths.cpp: Class to hold multiple Auth
    - Block.cpp: Block data structure
    - CBlock.cpp: Certified block data structure, using a CA struct, for a Certificate
    - CData.cpp: CData class, linking a Block or hash to a phase/view
    - Cert.cpp: Stores a number of signatures with a phase/view, symbolizing a certificate
    - Client.cpp: Client that can request transactions
    - config.h: all defines
    - FData.cpp: data class for new-view certificate
    - FJust.cpp: new-view certificate
    - FVJust.cpp: Justrifications from TEEStore
    - HAccum.cpp: Class for accumulator
    - Handler.cpp: Does all the replica work!
    - Hash.cpp: Creates hashes
    - HJust.cpp: prepare pre-certificate 
    - JBlock.cpp: Justified Block class,  for genesis block
    - Keys.cpp: generate different keys
    - KeysFun.cpp: keys functionality
    - Log.cpp: Log class for debug functionality
    - Message.h: defines struct for message passing
    - NodeInfo.cpp: Defines client port and replica port
    - OPaccum.cpp: Onephase accumulate certificate
    - OPprepare.cpp: Onephase prepare certificate
    - OPproposal.cpp: Onephase proposal 
    - OPstore.cpp: Onephase store
    - OPstoreCert.hcpp: Onephase store certificate 
    - OPvote.cpp: Onephase vote certificate
    - params.h: configures nodes, signature, transactions and payload numbers
    - PJust.cpp: prepare certificate
    - Proposal.cpp: proposal containing Block and Just
    - RData.cpp: class with info on a Round
    - Server.cpp: Defines size of messages based on params.h
    - Sign.cpp: Allows basis signing for a replica
    - Start.cpp: Allows for start of a round
    - test.cpp: test for new type of RSA and EC signatures
    - Transaction.h: class to symbolize transaction in the protocol
    - TrustedAccum.cpp: accumulator access to trusted component
    - TrustedCh.cpp: Trusted component for chained damysus
    - TrustedChComb.cpp: Trusted compontent for chained damysus combined
    - TrustedComb.cpp: Trusted component combined (non-chained)
    - TrustedFun.cpp:
    - TrustedFunC.c:
    - types_backup.h: stores enums for MODE and HEADER
    - types.h: create required defines
    - Value.h:
    - Vjust.cpp: Create vote justification
    - Void.cpp: Create empty justification
    - Vote.cpp: Create vote on some data

### 3. Components

Explain the major components/modules of the blockchain application.

### 4. Dependencies

List the external dependencies used in the blockchain application.

### 5. Configuration

Explain any configuration files or settings required for the blockchain application.

### 6. Usage

Provide instructions on how to use the blockchain application.

### 7. Examples

Include code examples or usage scenarios for better understanding.

### 8. Tests

Explain the testing strategy and provide instructions for running tests.

### 9. Troubleshooting

Include common issues and their solutions.

## SGX Trusted Component

### 1. Introduction

The SGX trusted component prohibits the equivocation, by only allowing one signature to be applied per view/phase combination. Asking for extra signatures will be ignored. It can also keep track of the latest prepared and locked block. Additionally, it can act as an accumulator, which allows a replica to independently verify the latest block out of several collected new-view messages. 

### 2. Directory Structure

Within the Enclave structure, an Enclave private RSA key is stored in Enclave_private.pem. EnclaveShare.h provides the methods, that are in different configurations implemented in all cpp files. 


### 3. Components

Explain the major components/modules of the SGX trusted component.

### 4. Dependencies

List the external dependencies used in the SGX trusted component.

### 5. Configuration

Explain any configuration files or settings required for the SGX trusted component.

### 6. Usage

Provide instructions on how to use the SGX trusted component.

### 7. Examples

Include code examples or usage scenarios for better understanding.

### 8. Tests

Explain the testing strategy and provide instructions for running tests.

### 9. Troubleshooting

Include common issues and their solutions.

