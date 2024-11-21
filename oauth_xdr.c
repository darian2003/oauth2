/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "oauth.h"

bool_t
xdr_auth_response (XDR *xdrs, auth_response *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->auth_token, 16))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->status))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_approve_request (XDR *xdrs, approve_request *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->auth_token, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->signature, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->permissions, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_access_request (XDR *xdrs, access_request *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->id, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->signature, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->auth_token, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_access_response (XDR *xdrs, access_response *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->access_token, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->refresh_token, ~0))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->ttl))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_action_request (XDR *xdrs, action_request *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->action, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->access_token, ~0))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->resource, ~0))
		 return FALSE;
	return TRUE;
}
