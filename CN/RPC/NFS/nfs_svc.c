/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "nfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
nfs_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		direntry nfs_ls_1_arg;
		readargs nfs_read_1_arg;
		writeargs nfs_write_1_arg;
		filelist nfs_touch_1_arg;
		filelist nfs_rm_1_arg;
		entry nfs_getattr_1_arg;
		sattrargs nfs_setattr_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case NFS_LS:
		_xdr_argument = (xdrproc_t) xdr_direntry;
		_xdr_result = (xdrproc_t) xdr_filelist;
		local = (char *(*)(char *, struct svc_req *)) nfs_ls_1_svc;
		break;

	case NFS_READ:
		_xdr_argument = (xdrproc_t) xdr_readargs;
		_xdr_result = (xdrproc_t) xdr_readres;
		local = (char *(*)(char *, struct svc_req *)) nfs_read_1_svc;
		break;

	case NFS_WRITE:
		_xdr_argument = (xdrproc_t) xdr_writeargs;
		_xdr_result = (xdrproc_t) xdr_writeres;
		local = (char *(*)(char *, struct svc_req *)) nfs_write_1_svc;
		break;

	case NFS_TOUCH:
		_xdr_argument = (xdrproc_t) xdr_filelist;
		_xdr_result = (xdrproc_t) xdr_status;
		local = (char *(*)(char *, struct svc_req *)) nfs_touch_1_svc;
		break;

	case NFS_RM:
		_xdr_argument = (xdrproc_t) xdr_filelist;
		_xdr_result = (xdrproc_t) xdr_status;
		local = (char *(*)(char *, struct svc_req *)) nfs_rm_1_svc;
		break;

	case NFS_GETATTR:
		_xdr_argument = (xdrproc_t) xdr_entry;
		_xdr_result = (xdrproc_t) xdr_attr;
		local = (char *(*)(char *, struct svc_req *)) nfs_getattr_1_svc;
		break;

	case NFS_SETATTR:
		_xdr_argument = (xdrproc_t) xdr_sattrargs;
		_xdr_result = (xdrproc_t) xdr_status;
		local = (char *(*)(char *, struct svc_req *)) nfs_setattr_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (NFS_PROG, NFS_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, NFS_PROG, NFS_VERS, nfs_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (NFS_PROG, NFS_VERS, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, NFS_PROG, NFS_VERS, nfs_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (NFS_PROG, NFS_VERS, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
