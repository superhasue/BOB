using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Security.Cryptography; 

namespace study301
{
    class DESEnDecoder : IEnDecoder
    {
        public string key { get; set; }
        public string IV { get; set; }
        public string format { get; set; }

        public DESEnDecoder()
        {
            key = string.Empty;
            IV = string.Empty;
            format = "Base64";
        }

        public DESEnDecoder(string _key, string _IV)
        {
            key = _key;
            IV = _IV;
            format = "Base64";
        }

        public void CreatingIV(string str)
        {
            char[] c1 = str.ToCharArray(); 
            Array.Reverse(c1);
            //String.ToCharArray - String 문자열을 char[] 으로 변경
            //Array.Reverse -  배열전체에 있는 요소의 시퀀스를 역순으로 설정

            //or

            char[] c2 = str.ToCharArray().Reverse().ToArray();
            //String.ToCharArray - String 문자열을 char[] 으로 변경
            //Enumerable.Reverse - 배열에서 요소의 순서를 바꿈.
            //Enumerable.ToArray - Enumerable 클래스의 확장 메서드는 IEnumerable 인터페이스를 반환한다. 하지만 코드를 작성하다보면 인터페이스가 아닌 구체화된 클래스가 필요할 때가 있다. 그 때 변환 연산자를 활용하여 Array 타입으로 변환할 수 있다.

            IV = new string(c1);
        }

        public string Encode(string sParam)
        {
            if (string.IsNullOrEmpty(sParam))
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            if (key == String.Empty || key.Length != 8)
                return sParam = "입력된 키가 올바르지 않습니다.";

            if (IV == String.Empty)
                CreatingIV(key);

            string str = String.Empty;

            DESCryptoServiceProvider desCSP = new DESCryptoServiceProvider();
            desCSP.KeySize = 64; //실제로는 패리티 비트가 8 비트 붙어있어서 실제 키 길이(=암호화 강도)는 56 비트이다. 
            desCSP.BlockSize = 64;
            desCSP.Mode = CipherMode.CBC; //이전 block의 암호화된 결과물을, 다음의 평문 block 과 XOR 연산 후 암호화. 이때 최초 block의 경우 XOR 연산을 수행할 대상인 이전 block이 없으므로, IV(Initial Vector)를 이용하여 연산
            //https://docs.microsoft.com/ko-kr/dotnet/api/system.security.cryptography.ciphermode?view=net-5.0
            desCSP.Padding = PaddingMode.PKCS7;
            //https://docs.microsoft.com/ko-kr/dotnet/api/system.security.cryptography.paddingmode?view=net-5.0

            desCSP.Key = Encoding.Default.GetBytes(key);
            desCSP.IV = Encoding.Default.GetBytes(IV);

            try
            {
                // 암호화는 스트림(바이트 배열)을 대칭키에 의존하여 암호화 하기때문에 먼저 메모리 스트림을 생성한다.
                MemoryStream ms = new MemoryStream();

                //만들어진 메모리 스트림을 이용해서 암호화 스트림 생성
                CryptoStream cs = new CryptoStream(ms, desCSP.CreateEncryptor(), CryptoStreamMode.Write);

                // 데이터를 바이트 배열로 변경
                byte[] data = Encoding.UTF8.GetBytes(sParam.ToCharArray());

                // 암호화 스트림에 데이터 씀
                cs.Write(data, 0, data.Length);
                cs.FlushFinalBlock();

                //Output Text Format
                if (format == "Base64")
                    str = Convert.ToBase64String(ms.ToArray());
                else if (format == "Hex")
                {
                    str = BitConverter.ToString(ms.ToArray());
                    str = str.Replace("-", "");
                } else { str = "포맷이 잘못되었습니다."; }

                cs.Close();
                ms.Close();

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

            if (key == String.Empty || key.Length != 8)
                return sParam = "입력된 키가 올바르지 않습니다.";

            if (IV == String.Empty)
                CreatingIV(key);

            string str = String.Empty;

            DESCryptoServiceProvider desCSP = new DESCryptoServiceProvider();
            desCSP.Mode = CipherMode.CBC;
            desCSP.Padding = PaddingMode.PKCS7;

            desCSP.Key = Encoding.Default.GetBytes(key);
            desCSP.IV = Encoding.Default.GetBytes(IV);

            try
            {
                //Input Text Format
                byte[] data;
                if (format == "Base64")
                    data = System.Convert.FromBase64String(sParam);
                else if (format == "Hex")
                {
                    data = new byte[sParam.Length / 2];
                    for (int i = 0; i < data.Length; i++)
                        data[i] = Convert.ToByte(sParam.Substring(i * 2, 2), 16);
                } else { str = "포맷이 잘못되었습니다."; return str; }

                MemoryStream ms = new MemoryStream(data);
                CryptoStream cs = new CryptoStream(ms, desCSP.CreateDecryptor(), CryptoStreamMode.Read);
                StreamReader sr = new StreamReader(cs);

                str = sr.ReadToEnd();
                
                cs.Close();
                sr.Close();
                ms.Close();

            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
            }


            return str;
        }

    }
}

//https://zinzza.tistory.com/20
//https://icechou.tistory.com/325
//https://dhshin94.tistory.com/119
//https://3dmpengines.tistory.com/2007

//Enumerable - 열거 가능한 
//Enumerator - 열거자 

//IEnumerable은 컬렉션의 인터페이스이고
//IEnumerator는 열거자의 인터페이스이다.

//C#에서 모든 컬렉션은 IEnumerable<T> 인터페이스를 구현한다. => 열거자를 꺼내는 GetEnumerator() 라는 메소드를 제공한다.
//열거자  => 컬렉션의 요소를 가리키는 객체
//        => MoveNext(), Current, Reset() 멤버로 모든 요소에 접근 가능
//        => 모든 열거자는 IEnumerator<T> 인터페이스를 구현하고 있다.

//List<int> lst = new List<int>(arr);
//IEnumerator<int> inmrt = lst.GetEnumerator();
//inmrt.MoveNext(); //최초 호출로 초기화
//Console.WriteLine(e1.Current);  // 첫 번째 요소
//inmrt.MoveNext(); // 다음으로 이동
//Console.WriteLine(e1.Current);  // 두 번째 요소

//사실 foreach문을 사용하면 내부적으로 열거자를 사용하는 코드로 바뀌게 된다.



//namespace System.Collections 
//public interface IEnumerable
//public interface IEnumerator

//namespace System.Collections.Generic
//public interface IEnumerable<out T> : System.Collections.IEnumerable
//public interface IEnumerator<out T> : IDisposable, System.Collections.IEnumerator

//IEnumerable.GetEnumerator 메서드 - 컬렉션을 반복하는 열거자를 반환합니다.
//IEnumerator.Current 속성 - 컬렉션에서 열거자의 현재 위치에 있는 요소를 가져옵니다.
//IEnumerator.MoveNext 메서드 - 열거자를 컬렉션의 다음 요소로 이동합니다.
//IEnumerator.Reset 메서드 - 컬렉션의 첫 번째 요소 앞의 초기 위치에 열거자를 설정합니다.