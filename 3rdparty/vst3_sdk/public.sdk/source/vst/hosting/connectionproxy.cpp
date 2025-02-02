//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/utility/connectionproxy.cpp
// Created by  : Steinberg, 04/2019
// Description : VST 3 Plug-in connection class
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2021, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation 
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this 
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "connectionproxy.h"

namespace Steinberg {
namespace Vst {

//------------------------------------------------------------------------
IMPLEMENT_FUNKNOWN_METHODS (ConnectionProxy, IConnectionPoint, IConnectionPoint::iid)

//------------------------------------------------------------------------
ConnectionProxy::ConnectionProxy (IConnectionPoint* srcConnection)
: srcConnection (srcConnection) // share it
{
	FUNKNOWN_CTOR
}

//------------------------------------------------------------------------
ConnectionProxy::~ConnectionProxy ()
{
	FUNKNOWN_DTOR
}

//------------------------------------------------------------------------
tresult PLUGIN_API ConnectionProxy::connect (IConnectionPoint* other)
{
	if (other == nullptr)
		return kInvalidArgument;
	if (dstConnection)
		return kResultFalse;

	dstConnection = other; // share it
	tresult res = srcConnection->connect (this);
	if (res != kResultTrue)
		dstConnection = nullptr;
	return res;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ConnectionProxy::disconnect (IConnectionPoint* other)
{
	if (!other)
		return kInvalidArgument;

	if (other == dstConnection)
	{
		if (srcConnection)
			srcConnection->disconnect (this);
		dstConnection = nullptr;
		return kResultTrue;
	}

	return kInvalidArgument;
}

//------------------------------------------------------------------------
tresult PLUGIN_API ConnectionProxy::notify (IMessage* message)
{
	if (dstConnection)
	{
		// We discard the message if we are not in the UI main thread
		if (threadChecker && threadChecker->test ())
			return dstConnection->notify (message);
	}
	return kResultFalse;
}

//------------------------------------------------------------------------
bool ConnectionProxy::disconnect ()
{
	return disconnect (dstConnection) == kResultTrue;
}

//------------------------------------------------------------------------
} // Vst
} // Steinberg

