using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Security.Cryptography;

namespace study301
{
    class HashConverter
    {
        private byte[] byteHashArray;
        private string strHashData;

        public HashConverter()
        {

        }

        public string MD5HashFile(string path) //path = filePath + fileName
        {
            FileStream fs = null;
            MD5CryptoServiceProvider md5 = new MD5CryptoServiceProvider();

            try
            {
                using (fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
                {
                    byteHashArray = md5.ComputeHash(fs);
                }

                strHashData = BitConverter.ToString(byteHashArray); //BitConverter : 바이트배열 변환클래스
                strHashData = strHashData.Replace("-", "");
            }
            catch (Exception ex)
            {
                strHashData = "오류발생 : " + ex.Message.ToString();
                //throw;
            }

            return strHashData;
        }

        public string MD5HashTxt(string txt)
        {
            MD5CryptoServiceProvider md5 = new MD5CryptoServiceProvider();

            //지정된 바이트 배열에 대해 해시값을 계산합니다.
            byteHashArray = md5.ComputeHash(Encoding.ASCII.GetBytes(txt));

            //바이트배열을 문자열로 변환합니다.
            strHashData = BitConverter.ToString(byteHashArray);
            strHashData = strHashData.Replace("-", "");

            return strHashData;
        }


        public string SHA1HashFile(string path)
        {
            FileStream fs = null;
            SHA1CryptoServiceProvider sha1 = new SHA1CryptoServiceProvider();

            try
            {
                using (fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
                {
                    byteHashArray = sha1.ComputeHash(fs);
                }

                strHashData = BitConverter.ToString(byteHashArray); //BitConverter : 바이트배열 변환클래스
                strHashData = strHashData.Replace("-", "");
            }
            catch (Exception ex)
            {
                strHashData = "오류발생 : " + ex.Message.ToString();
                //throw;
            }

            return strHashData;
        }

        public string SHA1HashTxt(string txt)
        {
            SHA1CryptoServiceProvider sha1 = new SHA1CryptoServiceProvider();

            //지정된 바이트 배열에 대해 해시 값을 계산합니다.
            byteHashArray = sha1.ComputeHash(Encoding.ASCII.GetBytes(txt));

            //바이트배열을 문자열로 변환합니다.
            strHashData = BitConverter.ToString(byteHashArray);
            strHashData = strHashData.Replace("-", "");

            return strHashData;
        }
    }
}

//참고 : https://nickname.tistory.com/25