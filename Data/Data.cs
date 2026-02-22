using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MeterCi62App.Data
{
    

    public struct Lab
    {
        public double L;
        public double a;
        public double b;

        public override string ToString() => $"L: {L}, a: {a}, b: {b}";
    }

    public enum Illuminant
    {
        D65_10,
        D75_10,
        F2_10,
        A_10,
        C_2,
    }



}
