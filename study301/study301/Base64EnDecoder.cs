using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace study301
{
    class Base64EnDecoder : IEnDecoder
    {
        public string key { get; set; }
        public string IV { get; set; }
        public string format { get; set; }

        public Base64EnDecoder()
        {

        }

        public string Encode(string sParam)
        {
            if (sParam == null || sParam == String.Empty)
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            string str = String.Empty;

            try
            {
                byte[] enbytes = Encoding.Default.GetBytes(sParam);
                str = Convert.ToBase64String(enbytes);
            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
                //throw;
            }

            return str;
        }


        public string Decode(string sParam)
        {
            if (sParam == null || sParam == String.Empty)
            {
                return sParam = "입력된 문자가 없습니다.";
            }

            string str = String.Empty;

            try
            {
                byte[] debytes = Convert.FromBase64String(sParam);
                str = Encoding.Default.GetString(debytes);
            }
            catch (Exception ex)
            {
                str = "오류발생 : " + ex.Message.ToString();
                //throw;
            }

            return str;
        }

    }
}
