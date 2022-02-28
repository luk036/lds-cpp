// #![feature(unboxed_closures)]

const TWO_PI: f64 = std::f64::consts::TAU;

fn vdc(mut k: usize, base: usize) -> f64 {
    let mut vdc = 0.0;
    let mut denom = 1.0;
    while k != 0 {
        denom *= base as f64;
        let remainder = k % base;
        k /= base;
        vdc += (remainder as f64) / denom;
    }
    vdc
}

pub struct Vdcorput {
    count: usize,
    base: usize,
}

impl Vdcorput {
    pub fn new(base: usize) -> Self {
        Vdcorput { count: 0, base }
    }

    pub fn new_default() -> Self {
        Vdcorput { count: 0, base: 2 }
    }

    pub fn pop(&mut self) -> f64 {
        self.count += 1;
        vdc(self.count, self.base)
    }

    #[allow(dead_code)]
    pub fn reseed(&mut self, seed: usize) {
        self.count = seed;
    }
}

// impl FnOnce<()> for Vdcorput {
//     type Output = f64;
//     extern "rust-call" fn call_once(self, _arg: ()) -> Self::Output {
//         self.count += 1;
//         vdc(self.count, self.base)
//     }
// }

/**
 * @brief Halton sequence generator
 *
 */
pub struct Halton {
    vdc0: Vdcorput,
    vdc1: Vdcorput,
}

impl Halton {
    pub fn new(base: &[usize]) -> Self {
        Halton {
            vdc0: Vdcorput::new(base[0]),
            vdc1: Vdcorput::new(base[1]),
        }
    }

    pub fn pop(&mut self) -> [f64; 2] {
        [self.vdc0.pop(), self.vdc1.pop()]
    }

    /**
     * @brief
     *
     * @param seed
     */
    #[allow(dead_code)]
    pub fn reseed(&mut self, seed: usize) {
        self.vdc0.reseed(seed);
        self.vdc1.reseed(seed);
    }
}

/**
 * @brief Circle sequence generator
 *
 */
pub struct Circle {
    vdc: Vdcorput,
}

impl Circle {
    pub fn new(base: usize) -> Self {
        Circle {
            vdc: Vdcorput::new(base),
        }
    }

    pub fn pop(&mut self) -> [f64; 2] {
        // let two_pi = 2.0 * (-1.0 as f64).acos(); // ???
        let theta = self.vdc.pop() * TWO_PI; // map to [0, 2*pi];
        [theta.sin(), theta.cos()]
    }

    #[allow(dead_code)]
    pub fn reseed(&mut self, seed: usize) {
        self.vdc.reseed(seed);
    }
}

/**
 * @brief Sphere sequence generator
 *
 */
pub struct Sphere {
    vdc: Vdcorput,
    cirgen: Circle,
}

impl Sphere {
    pub fn new(base: &[usize]) -> Self {
        Sphere {
            vdc: Vdcorput::new(base[0]),
            cirgen: Circle::new(base[1]),
        }
    }

    pub fn pop(&mut self) -> [f64; 3] {
        let cosphi = 2.0 * self.vdc.pop() - 1.0; // map to [-1, 1];
        let sinphi = (1.0 - cosphi * cosphi).sqrt();
        let [c, s] = self.cirgen.pop();
        [sinphi * c, sinphi * s, cosphi]
    }

    /**
     * @brief
     *
     * @param seed
     */
    #[allow(dead_code)]
    pub fn reseed(&mut self, seed: usize) {
        self.cirgen.reseed(seed);
        self.vdc.reseed(seed);
    }
}

/**
 * @brief S(3) sequence generator by Hopf
 *
 */
pub struct Sphere3Hopf {
    vdc0: Vdcorput,
    vdc1: Vdcorput,
    vdc2: Vdcorput,
}

impl Sphere3Hopf {
    pub fn new(base: &[usize]) -> Self {
        Sphere3Hopf {
            vdc0: Vdcorput::new(base[0]),
            vdc1: Vdcorput::new(base[1]),
            vdc2: Vdcorput::new(base[2]),
        }
    }

    pub fn pop(&mut self) -> [f64; 4] {
        let phi = self.vdc0.pop() * TWO_PI; // map to [0, 2*pi];
        let psy = self.vdc1.pop() * TWO_PI; // map to [0, 2*pi];
        let vd = self.vdc2.pop();
        let cos_eta = vd.sqrt();
        let sin_eta = (1.0 - vd).sqrt();
        [
            cos_eta * psy.cos(),
            cos_eta * psy.sin(),
            sin_eta * (phi + psy).cos(),
            sin_eta * (phi + psy).sin(),
        ]
    }

    #[allow(dead_code)]
    pub fn reseed(&mut self, seed: usize) {
        self.vdc0.reseed(seed);
        self.vdc1.reseed(seed);
        self.vdc2.reseed(seed);
    }
}

