/****************************************************************************
   Program:     $Id: $
   Date:        $Date: $
   Description: yarrp runtime configuration parsing
****************************************************************************/
#include "yarrp.h"
int verbosity;

static struct option long_options[] = {
    {"srcaddr", required_argument, NULL, 'a'},
    {"bgp", required_argument, NULL, 'b'},
    {"blocklist", required_argument, NULL, 'B'},
    {"coarse", required_argument, NULL, 'C'},
    {"count", required_argument, NULL, 'c'},
    {"fillmode", required_argument, NULL, 'F'},
    {"poisson", required_argument, NULL, 'Z'},
    {"srcmac", required_argument, NULL, 'M'},
    {"help", no_argument, NULL, 'h'},
    {"input", required_argument, NULL, 'i'},
    {"interface", required_argument, NULL, 'I'},
    {"maxttl", required_argument, NULL, 'm'},
    {"dstmac", required_argument, NULL, 'G'},
    {"neighborhood", required_argument, NULL, 'n'},
    {"output", required_argument, NULL, 'o'},
    {"port", required_argument, NULL, 'p'}, 
    {"probeonly", required_argument, NULL, 'P'}, 
    {"entire", no_argument, NULL, 'Q'},
    {"rate", required_argument, NULL, 'r'},
    {"receiveonly", required_argument, NULL, 'R'},
    {"sequential", no_argument, NULL, 's'},
    {"seed", required_argument, NULL, 'S'},
    {"type", required_argument, NULL, 't'},
    {"verbose", no_argument, NULL, 'v'},
    {"testing", no_argument, NULL, 'T'},
    {"instance", required_argument, NULL, 'E'}, 
    {NULL, 0, NULL, 0},
};

uint8_t *read_mac(char *str) {
    uint8_t *mac = (uint8_t *) malloc (6 * sizeof(uint8_t));
    mac[0] = (uint8_t) strtoul(strtok(str, ":"), NULL, 16);
    for (int i=1; i < 6; i++) 
        mac[i] = (uint8_t) strtoul(strtok(NULL, ":"), NULL, 16);
    return mac;
}

struct in6_addr *read_v6_src_addr(char *str) {
    in6_addr *src = (in6_addr *) malloc(sizeof(in6_addr));
    inet_pton(AF_INET6, str, src);
    return src;
}

void
YarrpConfig::parse_opts(int argc, char **argv) {
    int c, opt_index;
    char *endptr;

    if (argc <= 1)
        usage(argv[0]);
    type = TR_TCP_ACK;
    seed = time(NULL);
#ifdef GITREV
    params["Program"] = val_t("Yarrp v" + string(VERSION) + " (" + GITREV + ")", true);
#else
    params["Program"] = val_t("Yarrp v" + string(VERSION), true);
#endif
    params["RTT_Granularity"] = val_t("us", true);
    params["Targets"] = val_t("entire", true);
    while (-1 != (c = getopt_long(argc, argv, "a:b:B:c:CE:F:G:hi:I:m:M:n:o:p:P:Qr:RsS:t:vTZ:", long_options, &opt_index))) {
        switch (c) {
        case 'b':
            bgpfile = optarg;
            params["BGP_table"] = val_t(bgpfile, true);
            break;
        case 'B':
            blocklist = optarg;
            params["Blocklist"] = val_t(blocklist, true);
            break;
        case 'Z':
            poisson = strtol(optarg, &endptr, 10);
            params["Poisson"] = val_t(to_string(poisson), true);
            break;
        case 'C':
            coarse = true;
            params["RTT_Granularity"] = val_t("ms", true);
            break;
        case 'c':
            count = strtol(optarg, &endptr, 10);
            params["Count"] = val_t(to_string(count), true);
            break;
        case 'F':
            fillmode = strtol(optarg, &endptr, 10);
            break;
        case 'i':
            inlist = optarg;
            params["Targets"] = val_t(inlist, true);
            break;
        case 's':
            random_scan = false;
            params["Sequential"] = val_t("true", true);
            break;
        case 'S':
            seed = strtol(optarg, &endptr, 10);
            break;
        case 'T':
            testing = true;
            break;
        case 'Q':
            entire = true;
            break;
        case 'n':
            ttl_neighborhood = strtol(optarg, &endptr, 10);
            break;
        case 'v':
            verbosity++;
            break;
        case 'o':
            output = optarg;
            params["Output"] = val_t(output, true);
            break;
        case 'p':
            dstport = strtol(optarg, &endptr, 10);
            break;
        case 'E':
            instance = strtol(optarg, &endptr, 10);
            params["Instance"] = val_t(to_string(instance), true);
            break;
        case 'P':
            probesrc = optarg;
            receive = false;
            break;
        case 'R':
            probe = false;
            break;
        case 'm':
            maxttl = strtol(optarg, &endptr, 10);
            break;
        case 'r':
            rate = strtol(optarg, &endptr, 10);
            break;
        case 'I':
            int_name = optarg;
            break;
        case 'G':
            dstmac = read_mac(optarg);
            break;
        case 'M':
            srcmac = read_mac(optarg);
            break;
        case 'a':
            srcaddr = read_v6_src_addr(optarg);
            break;
        case 't':
            if (strcmp(optarg, "ICMP6") == 0) {
                ipv6 = true;
                type = TR_ICMP6;
            } else if(strcmp(optarg, "UDP6") == 0) {
                ipv6 = true;
                type = TR_UDP6;
            } else if(strcmp(optarg, "TCP6_SYN") == 0) {
                ipv6 = true;
                type = TR_TCP6_SYN;
            } else if(strcmp(optarg, "TCP6_ACK") == 0) {
                ipv6 = true;
                type = TR_TCP6_ACK;
            } else if(strcmp(optarg, "ICMP") == 0) {
                type = TR_ICMP;
            } else if(strcmp(optarg, "ICMP_REPLY") == 0) {
                type = TR_ICMP_REPLY;
            } else if(strcmp(optarg, "UDP") == 0) {
                type = TR_UDP;
            } else if(strcmp(optarg, "TCP_SYN") == 0) {
                type = TR_TCP_SYN;
            } else if(strcmp(optarg, "TCP_ACK") == 0) {
                type = TR_TCP_ACK;
            } else {
                usage(argv[0]);
            }
            break;
        case 'h':
        default:
            debug(OFF, ">> yarrp v" << VERSION);
            debug(OFF, ">> https://www.cmand.org/yarrp/");
            usage(argv[0]);
        }
    }
    if (not testing) {
        /* set default output file, if not set */
        if (not output) {
            output = (char *) malloc(UINT8_MAX);
            snprintf(output, UINT8_MAX, "output.yrp");
        }
        debug(DEBUG, ">> Output: " << output);
        /* set output file */
        if ( (output)[0] == '-')
            out = stdout;
        else
            out = fopen(output, "a");
        if (out == NULL)
            fatal("%s: cannot open %s: %s", __func__, output, strerror(errno));
    }

    /* set default destination port based on tracetype, if not set */
    if (not dstport) {
        dstport = 80;
        if ( (type == TR_UDP) || (type == TR_UDP6) )
            dstport = 53;
    }
    debug(LOW, ">> yarrp v" << VERSION);

    params["Seed"] = val_t(to_string(seed), true);
    params["Random"] = val_t(to_string(random_scan), true);
    params["Rate"] = val_t(to_string(rate), true);
    params["Trace_Type"] = val_t(Tr_Type_String[type], true);
    params["Start"] = val_t("unknown", true);
    params["Fill_Mode"] = val_t(to_string(fillmode), true);
    params["Max_TTL"] = val_t(to_string(maxttl), true);
    params["TTL_Nbrhd"] = val_t(to_string(ttl_neighborhood), true);
    params["Dst_Port"] = val_t(to_string(dstport), true);
    params["Output_Fields"] = val_t("target sec usec type code ttl hop rtt ipid psize rsize rttl rtos count", true);
}


void YarrpConfig::set(string key, string val, bool isset) {
    params[key] = val_t(val, isset);
}

void
YarrpConfig::dump(FILE *fd) {
    for (params_t::iterator i = params.begin(); i != params.end(); i++ ) {
        string key = i->first;
        val_t val = i->second;
        if (val.second)
            fprintf(fd, "# %s: %s\n", key.c_str(), val.first.c_str());
    }
    fflush(fd);
}


void
YarrpConfig::usage(char *prog) {
    cout << "Usage: " << prog << " [OPTIONS] [targets]" << endl
    << "OPTIONS:" << endl
    << "  -i, --input             Input target file" << endl
    << "  -o, --output            Output file (default: output.yrp)" << endl
    << "  -c, --count             Probes to issue (default: unlimited)" << endl
    << "  -t, --type              Probe type: ICMP, ICMP_REPLY, TCP_SYN, TCP_ACK, UDP," << endl
    << "                                      ICMP6, UDP6, TCP6_SYN, TCP6_ACK (default: TCP_ACK)" << endl
    << "  -r, --rate              Scan rate in pps (default: 10)" << endl
    << "  -m, --maxttl            Maximum TTL (default: 16)" << endl
    << "  -v, --verbose           verbose (default: off)" << endl
    << "  -F, --fillmode          Fill mode maxttl (default: 32)" << endl
    << "  -s, --sequential        Scan sequentially (default: random)" << endl
    << "  -n, --neighborhood      Neighborhood TTL (default: 0)" << endl
    << "  -b, --bgp               BGP table (default: none)" << endl
    << "  -B, --blocklist         Prefix Blocklist (default: none)" << endl
    << "  -S, --seed              Seed (default: random)" << endl
    << "  -p, --port              Transport dst port (default: 80)" << endl
    << "  -E, --instance          Prober instance (default: 0)" << endl
    << "  -T, --test              Don't send probes (default: off)" << endl
    << "  -Q, --entire            Entire IPv4/IPv6 Internet (default: off)" << endl
    << "  -Z, --poisson           Poisson TTLs (default: uniform)" << endl
    << "  -I, --interface         Network interface (required for IPv6)" << endl
    << "  -a, --srcaddr           IPv6 address of probing host (default: auto)" << endl
    << "  -G, --dstmac            MAC of gateway router (default: auto)" << endl
    << "  -M, --srcmac            MAC of probing host (default: auto)" << endl
    << "  -h, --help              Show this message" << endl
/* Undocumented options */
//    << "  -C, --coarse            Coarse ms timestamps (default: us)" << endl
//    << "  -P, --probeonly         Probe only, don't receive" << endl
//    << "  -R, --receiveonly       Receive only, don't probe" << endl
    << "Targets:" << endl
    << "  List of IPv4 or IPv6 prefixes." << endl
    << "    Example: 192.168.1.0/24" << endl
    << "             2602:306:8b92:b000::/47" << endl
    << endl;
    exit(-1);
}
