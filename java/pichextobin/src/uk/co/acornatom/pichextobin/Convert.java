package uk.co.acornatom.pichextobin;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.util.HashSet;
import java.util.Set;

public class Convert {
	
	public Convert() {
	}


	private void convertToAtm(File srcFile, File dstFile) throws IOException {

		
		byte[] memory = new byte[0x10000];
		for (int i = 0; i < memory.length; i++) {
			memory[i] = (byte) 255;
		}
		int last = 0;
		
		FileInputStream fis = null;
		BufferedReader br = null;
		try {
			String line;
			fis = new FileInputStream(srcFile);
			br = new BufferedReader(new InputStreamReader(fis, Charset.forName("UTF-8")));

			int linenum = 1;
			while ((line = br.readLine()) != null) {
				line = line.trim();
				if (!line.startsWith(":")) {
					throw new RuntimeException("Missing colon at line " + linenum);
				}
				
				// :08100800DACFE4FFE2CFDAFFCA
				// :LLAAAARRHHHHHHHHHHHHHHHHCC

				int total = 0;

				int len = Integer.parseInt(line.substring(1, 3), 16);
				if (line.length() != len * 2 + 11) {
					throw new RuntimeException("Incorrect line length at line " + linenum);					
				}
				

				int address = Integer.parseInt(line.substring(3, 7), 16);

				int record = Integer.parseInt(line.substring(7, 9), 16);
				if (record != 0) {
					System.out.println("Skipping record " + record + " at line " + linenum);
					continue;
				}
				
				int checksum = Integer.parseInt(line.substring(9 + len * 2, 11 + len * 2), 16);
				
				for (int i = 0; i < len; i++) {
					int data = Integer.parseInt(line.substring(9 + 2 * i, 11 + 2 * i), 16);
					total += data;
					memory[address + i] = (byte) data;
					if (address + i > last) {
						last = address + i;
					}
				}
				
				total += len;
				total += address;
				total += address >> 8;
				total += record;
				total += checksum;
				
				total &= 255;
				if (total != 0) {
					throw new RuntimeException("Incorrect checksum at line " + linenum);					
				}
				
				linenum++;
			}
			
			
	
				
		} finally {
			if (br != null) {
				br.close();
			}
			if (fis != null) {
				fis.close();
			}
		}

		// Round up to the next whole page
		last = (last + 0xff) & 0xFF00;

		// Output the data with out a checksum
		
		ByteArrayOutputStream bos = new ByteArrayOutputStream();
		
		bos.write('S');
		bos.write('M');
		bos.write('B');
		bos.write('!');
		bos.write(255);
		bos.write(255);
		for (int i = 0; i < 0x200 - 6; i++) {
			bos.write(0);
		}
		for (int i = 0x1000; i < last; i++) {
			bos.write(memory[i]);
		}
		bos.close();		
		
		byte[] binary = bos.toByteArray();
		
		// Calculate the CRC
		int crc = crc(binary, 512, 8192 + 512);

				
		binary[4] = (byte) (crc & 255);
		binary[5] = (byte) ((crc >> 8) & 255);
		
		FileOutputStream fos = new FileOutputStream(dstFile);
		fos.write(binary);
		fos.close();
	}
	
	private int crc(byte[] bytes, int start, int end) {
		int crc = 0xffff;          // initial value
		int polynomial = 0x1021;   // 0001 0000 0010 0001  (0, 5, 12) 


		for (int i = start; i < end; i++) {
			byte b = bytes[i];
            for (int j = 0; j < 8; j++) {
                boolean bit = ((b   >> (7-j) & 1) == 1);
                boolean c15 = ((crc >> 15    & 1) == 1);
                crc <<= 1;
                if (c15 ^ bit) crc ^= polynomial;
             }
		}
		crc &= 0xffff;
		System.out.println("CRC16-CCITT = " + Integer.toHexString(crc));
		return crc;
	}

	public static final void main(String[] args) {
		try {
			if (args.length != 2) {
				System.err.println("usage: java -jar pichextobin.jar <PIC Hex File File> <PIC Bin File>");
				System.exit(1);
			}
			File srcFile = new File(args[0]);
			if (!srcFile.exists()) {
				System.err.println("PIC Hex File: " + srcFile + " does not exist");
				System.exit(1);
			}
			if (!srcFile.isFile()) {
				System.err.println("PIC Hex File: " + srcFile + " is not a file");
				System.exit(1);
			}
			File dstFile = new File(args[1]);
			

			Convert c = new Convert();
			c.convertToAtm(srcFile, dstFile);

		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
