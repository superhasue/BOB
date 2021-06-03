using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Security;
using System.Security.Cryptography;
using System.Runtime.Serialization; // IFormatter 인터페이스 사용을 위해 추가 
using System.Runtime.Serialization.Formatters.Binary;
//이진 직렬화(Binary Serialization)는 이진 인코딩을 사용하여 저장소 또는 소켓 기반 네트워크 스트림과 같은 용도에서 사용할 수 있는 압축된 직렬화(Serialization)를 생성합니다.
using System.Xml.Serialization;
//XML 직렬화(XML Serialization)는 개체의 public 필드와 속성 또는 메서드의 매개 변수와 반환 값을 특정 XSD(XML 스키마 정의 언어) 문서와 일치하는 XML 스트림으로 직렬화



//직렬화

//직렬화는 개체(Object)(클래스/구조체/열거형/델리게이트)를 메모리나 파일에 저장 가능하도록 바이너리화(0과 1인 이진 형식/ JSON, XML, 등 텍스트 형식도 가능) 하는 것을 말한다. 
//직렬화를 하는 이유는 개체를 저장하거나 소켓 통신을 통해 전송하기 위해서 사용한다.
//Serializable 애트리뷰트를 선언하면 그 개체는 메모리에 저장 가능한 형식이 된다.
//직렬화란 객체를‘연속적인 데이터' 로 변환해서 저장 할 수 있게 하는것으로 스트림에서 읽고 쓰는것이 가능하다.
//직렬화란 객체의 인스턴스변수들의 값을 일렬로 나열하는 것이고, 객체를 저장한다는 것은 객체의 모든 인스턴스변수의 값을 저장하는 것이다.


//직렬화의 단점

//직렬화는 클래스의 상태를 가장 편하게 데이터화 할 수 있으나 몇가지 단점이 있습니다.
//첫번째는 이 직렬화된 데이터를 메모장에서 열어보면 사람이 이해하기 힘든 구조로 되어있습니다. 즉, 직렬화된 데이터는 역직렬화를 하기 전까지는 데이터를 알 수가 없습니다.
//그의 반해 json은 String 형태로 되어 있기 때문에 메모장으로 열어도 확인이 가능하고 직접 수정도 가능합니다.
//두번째는 플렛폼의 호환성을 좋지 않습니다. 즉, java에서 직렬화 시킨 데이터는 C#에서 역직렬화가 되지 않습니다. 
//정확하게는 되지 않는 것은 아닙니다만, 정보의 유실이 발생할 수 있습니다. 그리고 클래스가 변경되면 역직렬화시에 데이터 유실이 생기기 때문에 직렬화를 잘 사용하지를 않습니다.


//직렬화의 유형

//직렬화는 다음과 같은 유형으로 나뉘어 집니다.
//Binary Seralization
//SOAP Serialization
//XML Serialization
//Custom Serialization
//Binary Serialization
//참고 : https://oversky.tistory.com/53
//참고 : https://m.blog.naver.com/gmldbsdl6/80195745104


namespace study301
{
    class RSAEnDecoder : IEnDecoder
    {
        public string key { get; set; }
        public string IV { get; set; }
        public string format { get; set; }

        public string privateKeyText { get; set; }
        public string publicKeyText { get; set; }

        public RSAEnDecoder()
        {
            format = "Base64";
        }

        public void MakingKey()
        {
            //https://docs.microsoft.com/ko-kr/dotnet/api/system.security.cryptography.rsaparameters?view=net-5.0
            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();

            // 개인키 생성(복호화용)
            RSAParameters privateKey = RSA.Create().ExportParameters(true);
            rsa.ImportParameters(privateKey);
            privateKeyText = rsa.ToXmlString(true);

            // 공개키 생성(암호화용)
            RSAParameters publicKey = new RSAParameters();
            publicKey.Modulus = privateKey.Modulus;
            publicKey.Exponent = privateKey.Exponent;
            rsa.ImportParameters(publicKey);
            publicKeyText = rsa.ToXmlString(false);

        }

        public string Encode(string sParam)
        {
            if (string.IsNullOrEmpty(sParam))
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            string str = String.Empty;

            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();

            try
            {
                rsa.FromXmlString(publicKeyText);

                //암호화할 문자열을 UFT8 인코딩
                byte[] data = Encoding.UTF8.GetBytes(sParam.ToCharArray());

                //암호화
                byte[] enbyte = rsa.Encrypt(data, false);

                //Output Text Format
                if (format == "Base64")
                    str = Convert.ToBase64String(enbyte);
                else if (format == "Hex")
                {
                    str = BitConverter.ToString(enbyte);
                    str = str.Replace("-", "");
                } else { str = "포맷이 잘못되었습니다."; }

            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
            }
          
            return str;
        
        }

        public string Decode(string sParam)
        {
            if (string.IsNullOrEmpty(sParam))
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            string str = String.Empty;

            RSACryptoServiceProvider rsa = new RSACryptoServiceProvider();

            try
            {
                rsa.FromXmlString(privateKeyText);

                //Input Text Format
                byte[] data;
                if (format == "Base64")
                    data = System.Convert.FromBase64String(sParam);
                else if (format == "Hex")
                {
                    data = new byte[sParam.Length / 2];
                    for (int i = 0; i < data.Length; i++)
                        data[i] = Convert.ToByte(sParam.Substring(i * 2, 2), 16);
                }
                else { str = "포맷이 잘못되었습니다."; return str; }

                //복호화
                byte[] debyte = rsa.Decrypt(data, false);

                //복호화 바이트배열을 문자열로 변환
                str = Encoding.UTF8.GetString(debyte, 0, debyte.Length);

                //복호화 바이트배열을 문자열로 변환
                //str = (new UTF8Encoding()).GetString(debyte, 0, debyte.Length);

            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
            }

            return str;
        }
    }
}



