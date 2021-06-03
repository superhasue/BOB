using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace study301
{
    interface IEnDecoder
    {
        string key { get; set; }
        string IV { get; set; }
        string format { get; set; }

        string Encode(string sParam);
        string Decode(string sParam);
    }

}
