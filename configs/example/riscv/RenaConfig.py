import argparse

import m5
from m5.defines import buildEnv
from m5.objects import *

from common.Benchmarks import *
from common import ObjectList


def addRenaConfigOptions(parser):
    parser.add_argument( "--decodeToFetchDelay"     , type=int , default=1  , help="Decode to fetch delay")
    parser.add_argument( "--renameToFetchDelay"     , type=int , default=1  , help="Rename to fetch delay")
    parser.add_argument( "--iewToFetchDelay"        , type=int , default=1  , help="Issue/Execute/Writeback to fetch delay")
    parser.add_argument( "--commitToFetchDelay"     , type=int , default=1  , help="Commit to fetch delay")
    parser.add_argument( "--fetchWidth"             , type=int , default=8  , help="Fetch width")
    parser.add_argument( "--fetchBufferSize"        , type=int , default=64 , help="Fetch buffer size in bytes")
    parser.add_argument( "--fetchQueueSize"         , type=int , default=32 , help="Fetch queue size in micro-ops per-thread")
    parser.add_argument( "--renameToDecodeDelay"    , type=int , default=1  , help="Rename to decode delay")
    parser.add_argument( "--iewToDecodeDelay"       , type=int , default=1  , help="Issue/Execute/Writeback to decode delay")
    parser.add_argument( "--commitToDecodeDelay"    , type=int , default=1  , help="Commit to decode delay")
    parser.add_argument( "--fetchToDecodeDelay"     , type=int , default=1  , help="Fetch to decode delay")
    parser.add_argument( "--decodeWidth"            , type=int , default=4  , help="Decode width")
    parser.add_argument( "--iewToRenameDelay"       , type=int , default=1  , help="Issue/Execute/Writeback to rename delay")
    parser.add_argument( "--commitToRenameDelay"    , type=int , default=1  , help="Commit to rename delay")
    parser.add_argument( "--decodeToRenameDelay"    , type=int , default=1  , help="Decode to rename delay")
    parser.add_argument( "--renameWidth"            , type=int , default=4  , help="Rename width")
    parser.add_argument( "--commitToIEWDelay"       , type=int , default=1  , help="Commit to Issue/Execute/Writeback delay")
    parser.add_argument( "--renameToIEWDelay"       , type=int , default=1  , help="Rename to Issue/Execute/Writeback delay")
    parser.add_argument( "--issueToExecuteDelay"    , type=int , default=1  , help="Issue to execute delay (internal to the IEW stage)")
    parser.add_argument( "--dispatchWidth"          , type=int , default=4  , help="Dispatch width")
    parser.add_argument( "--issueWidth"             , type=int , default=4  , help="Issue width"   )
    parser.add_argument( "--wbWidth"                , type=int , default=8  , help="Writeback width")
    parser.add_argument( "--iewToCommitDelay"       , type=int , default=1  , help="Issue/Execute/Writeback to commit delay")
    parser.add_argument( "--renameToROBDelay"       , type=int , default=1  , help="Rename to reorder buffer delay")
    parser.add_argument( "--commitWidth"            , type=int , default=4  , help="Commit width")
    parser.add_argument( "--squashWidth"            , type=int , default=4  , help="Squash width")
    parser.add_argument( "--trapLatency"            , type=int , default=13 , help="Trap latency")
    parser.add_argument( "--fetchTrapLatency"       , type=int , default=1  , help="Fetch trap latency")
    parser.add_argument( "--backComSize"            , type=int , default=5  , help="Time buffer size for backwards communication")
    parser.add_argument( "--forwardComSize"         , type=int , default=5  , help="Time buffer size for forward communication")
    parser.add_argument( "--LQEntries"              , type=int , default=32 , help="Number of load queue entries")
    parser.add_argument( "--SQEntries"              , type=int , default=32 , help="Number of store queue entries")
    parser.add_argument( "--LSQDepCheckShift"       , type=int , default=4 , help="Number of places to shift addr before check")
    parser.add_argument( "--store_set_clear_period" , type=int , default=250000 ,help="Number of load/store insts before the dep predictor should be invalidated")
    parser.add_argument( "--LFSTSize"               , type=int , default=1024 ,help="Last fetched store table size")
    parser.add_argument( "--SSITSize"               , type=int , default=1024 ,help="Store set ID table size")
    parser.add_argument( "--numRobs"                , type=int , default=1 ,help="Number of Reorder Buffers")
    parser.add_argument( "--numPhysIntRegs"         , type=int , default=160 ,help="Number of physical integer registers")
    parser.add_argument( "--numPhysFloatRegs"       , type=int , default=160 ,help="Number of physical floating point registers")
    parser.add_argument( "--numPhysVecRegs"         , type=int , default=160 ,help="Number of physical vector registers")
    parser.add_argument( "--numPhysVecPredRegs"     , type=int , default=32 ,help="Number of physical predicate " "registers")
    parser.add_argument( "--numPhysCCRegs"          , type=int , default=0 ,help="Number of physical cc registers")
    parser.add_argument( "--numIQEntries"           , type=int , default=64 ,help="Number of instruction queue entries")
    parser.add_argument( "--numROBEntries"          , type=int , default=128 ,help="Number of reorder buffer entries")