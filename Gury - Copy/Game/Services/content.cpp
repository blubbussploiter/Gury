#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "../Gury/Game/strings.h"
#include "../Gury/Game/Serializer/decode.h"
#include "../Gury/Application/md5.h"

#include "content.h"

#include "stdout.h"

std::string RBX::Content::resolveUrl()
{
	if (isStored && !contentUrl.empty())
	{
		Url url = Url::Parse(contentUrl);

		if (url.Protocol.empty())
		{
			return contentUrl;
		}
		else if (url.Protocol == "rbxasset")
		{
			std::string file = url.Host;
			return ConFileInPath(RBX::Format("/content/%s", file.c_str()));
		}
		else if (url.Protocol == "rbxassetid")
		{
			/* From asset delivery? */
		}

	}
	return contentUrl;
}

bool RBX::Content::resolve()
{
	if (isBinary)
	{
		std::string data = base64_decode(binary);
		contentLength = data.length();

		content = new G3D::uint8[contentLength];
		std::copy(data.begin(), data.end(), content);

		return true;
	}

	if (isStored)
	{
		if (!contentUrl.empty()) {

			std::string fullUrl = resolveUrl();
			std::ifstream contentStream(fullUrl, std::ios::binary);

			Log::writeEntry(RBX::Format("%s resolving\n", fullUrl.c_str()).c_str());

			if (contentStream.good()) {

				contentStream.seekg(0, std::ios::end);
				contentLength = contentStream.tellg();
				contentStream.seekg(0, std::ios::beg);

				content = new G3D::uint8[contentLength];

				if (!contentStream.read((char*)content, contentLength)) {
					return false;
				}

				contentStream.close();
			}

			return true;
		}
	}

	if (isOnline) {

	}

	return false;
}

std::string RBX::Content::resolveContentHash()
{
	std::string base64 = base64_encode(content, contentLength);
	return md5(base64);
}
