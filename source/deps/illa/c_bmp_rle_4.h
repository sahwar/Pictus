#ifndef ILLA_C_BMP_RLE4_H
#define ILLA_C_BMP_RLE4_H

#include "c_bmp_rle.h"

namespace Img
{
	namespace Internal
	{
		class BmpRleDecoder4 :public BmpRleDecoder {
		public:
			BmpRleDecoder4(std::shared_ptr<Img::Surface> destination, const BMPHeader& header, std::shared_ptr<IO::FileReader> fileStream);

		private:
			bool OnProcess() override;
			std::shared_ptr<IO::FileReader> m_reader;
		};
	}
}

#endif
#pragma once
