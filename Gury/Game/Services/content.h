#pragma once

#include <string>
#include <algorithm>

#include <G3D/GImage.h>
#include <G3D/BinaryOutput.h>

#include "../Gury/Game/Serializer/encode.h"

namespace RBX
{
	enum MimeTypes
	{
		Mesh,
		Png,
		Jpg,
		Bmp,
		Tga,
		Mp3,
		Wav
	};

	/* stolen from https://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform */
	class Url
	{
	public:
		std::string QueryString, Path, Protocol, Host, Port;

		static Url Parse(const std::string& uri)
		{
			Url result;

			typedef std::string::const_iterator iterator_t;

			if (uri.length() == 0)
				return result;

			iterator_t uriEnd = uri.end();

			// get query start
			iterator_t queryStart = std::find(uri.begin(), uriEnd, '?');

			// protocol
			iterator_t protocolStart = uri.begin();
			iterator_t protocolEnd = std::find(protocolStart, uriEnd, ':');            //"://");

			if (protocolEnd != uriEnd)
			{
				std::string prot = &*(protocolEnd);
				if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
				{
					result.Protocol = std::string(protocolStart, protocolEnd);
					protocolEnd += 3;   //      ://
				}
				else
					protocolEnd = uri.begin();  // no protocol
			}
			else
				protocolEnd = uri.begin();  // no protocol

			// host
			iterator_t hostStart = protocolEnd;
			iterator_t pathStart = std::find(hostStart, uriEnd, '/');  // get pathStart

			iterator_t hostEnd = std::find(protocolEnd,
				(pathStart != uriEnd) ? pathStart : queryStart,
				':');  // check for port

			result.Host = std::string(hostStart, hostEnd);

			// port
			if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))  // we have a port
			{
				hostEnd++;
				iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
				result.Port = std::string(hostEnd, portEnd);
			}

			// path
			if (pathStart != uriEnd)
				result.Path = std::string(pathStart, queryStart);

			// query
			if (queryStart != uriEnd)
				result.QueryString = std::string(queryStart, uri.end());

			return result;

		}   // Parse
	};  // url

	class Content
	{
	public:
		bool isBinary;
		bool isStored; /* on disk */
		bool isOnline;/* on web */
		bool isEmpty;

		MimeTypes contentType;

		std::string binary;
		std::string internalContentUrl, contentUrl;

	public:

		G3D::uint8* content;
		size_t contentLength;

		std::string resolveUrl();

		bool resolve();

		std::string resolveContentHash();

		static Content fromBinary(std::string binary)
		{
			Content c;
			c.isEmpty = 0;
			c.isBinary = true;
			c.binary = binary;
			return c;
		}

		static Content fromStoredContent(std::string content)
		{
			Content c;
			c.isEmpty = 0;
			c.isStored = true;
			c.contentUrl = content;
			return c;
		}

		Content() : isBinary(0), isStored(0), isEmpty(1) {}
	};

	class ContentProvider
	{
	public:
		static ContentProvider* get();
	};
}
