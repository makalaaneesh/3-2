/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "nfs.h"

bool_t
xdr_readargs (XDR *xdrs, readargs *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->fhandle))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->filename, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->count))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_readres (XDR *xdrs, readres *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->data, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_writeargs (XDR *xdrs, writeargs *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->fhandle))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->filename, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->count))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->data, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_writeres (XDR *xdrs, writeres *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_direntry (XDR *xdrs, direntry *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->dirname, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_entry (XDR *xdrs, entry *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->filename, ~0))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->nextEntry, sizeof (entry), (xdrproc_t) xdr_entry))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_filelist (XDR *xdrs, filelist *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)&objp->entries, sizeof (entry), (xdrproc_t) xdr_entry))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_attr (XDR *xdrs, attr *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, &objp->mode))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_sattrargs (XDR *xdrs, sattrargs *objp)
{
	register int32_t *buf;

	 if (!xdr_entry (xdrs, &objp->file))
		 return FALSE;
	 if (!xdr_attr (xdrs, &objp->attrs))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_status (XDR *xdrs, status *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->val))
		 return FALSE;
	return TRUE;
}
