const polynomialOperation = {
  add(variable) {
    return (poly1, poly2) => this.processTerms(variable, poly1, poly2, (a, b) => a + b);
  },
  
  sub(variable) {
    return (poly1, poly2) => this.processTerms(variable, poly1, poly2, (a, b) => a - b);
  },
  
  mul(variable) {
    return (poly1, poly2) => {
      const terms1 = poly1.split('+').map(part => part.trim());
      const terms2 = poly2.split('+').map(part => part.trim());
      const resultTerms = {};
      terms1.forEach(termA => {
        const [coeffA, powerA] = this.splitTerm(termA, variable);
        terms2.forEach(termB => {
          const [coeffB, powerB] = this.splitTerm(termB, variable);
          const newPower = powerA + powerB;
          const newCoeff = coeffA * coeffB;
          if (resultTerms[newPower] === void 0) {
            resultTerms[newPower] = newCoeff;
          } else {
            resultTerms[newPower] += newCoeff;
          }
        });
      });

      return Object.keys(resultTerms)
        .sort((a, b) => b - a)
        .map(power => {
          const coeff = resultTerms[power];
          if (power == 0) return `${coeff}`;
          if (coeff === 1) return `${variable}${power}`;
          if (coeff === -1) return `-${variable}${power}`;
          return `${coeff}${variable}${power}`;
        })
        .join('+').replace(/\+\-/g, '-');
    };
  },
  dif(variable) {
    return expression => {
      if (expression.includes('(')) {
        const calResult = this.cal(variable)(expression);
        expression = calResult;
      }

      const terms = expression.split('+').map(part => part.trim());

      const differentiatedTerms = terms.map(term => {
        const [coeff, power] = this.splitTerm(term, variable);

        if (power === 0) {
          return '';
        } else {
          const newCoeff = coeff * power;
          const newPower = power - 1;

          if (newPower === 0) {
            return `${newCoeff}`;
          } else {
            return `${newCoeff}${variable}${newPower}`;
          }
        }
      });

      return differentiatedTerms.filter(term => term).join('+').replace(/\++/g, '+');
    };
  },
  cal(variable) {
    return expression => {
      expression = expression.replace(/\s/g, '');

      if (expression.includes('+(')) {
        const [poly1, poly2] = this.extractPolynomials(expression, '+');
        return this.add(variable)(poly1, poly2);
      } else if (expression.includes('-(')) {
        const [poly1, poly2] = this.extractPolynomials(expression, '-');
        return this.sub(variable)(poly1, poly2);
      } else if (expression.includes(')(')) {
        const [poly1, poly2] = this.extractPolynomials(expression, '*');
        return this.mul(variable)(poly1, poly2);
      } else {
        throw new Error('Unknown operation');
      }
    };
  },

  extractPolynomials(expression, operation) {
    let operatorIndex;
    switch (operation) {
      case '+':
        operatorIndex = expression.indexOf('+(');
        break;
      case '-':
        operatorIndex = expression.indexOf('-(');
        break;
      case '*':
        operatorIndex = expression.indexOf(')(');
        break;
      default:
        throw new Error('Invalid operation');
    }
    const poly1 = expression.substring(1, operatorIndex);
    const poly2 = expression.substring(operatorIndex + 2, expression.length - 1);
    return [poly1, poly2];
  },
  processTerms(variable, poly1, poly2, operationFn) {
    const terms1 = poly1.split('+').map(term => this.splitTerm(term, variable));
    const terms2 = poly2.split('+').map(term => this.splitTerm(term, variable));
    const resultTerms = {};
    
    terms1.forEach(term => {
      resultTerms[term[1]] = term[0];
    });

    terms2.forEach(term => {
      if (resultTerms[term[1]] === void 0) {
        resultTerms[term[1]] = operationFn(0, term[0]);
      } else {
        resultTerms[term[1]] = operationFn(resultTerms[term[1]], term[0]);
      }
    });

    return Object.keys(resultTerms)
      .sort((a, b) => b - a)
      .map(power => {
        if (resultTerms[power] === 0) return '';
        if (power == 0) return `${resultTerms[power]}`;
        if (resultTerms[power] === 1) return `${variable}${power}`;
        if (resultTerms[power] === -1) return `-${variable}${power}`;
        return `${resultTerms[power]}${variable}${power}`; 
      })
      .join('+').replace(/\+\-/g, '-').replace(/\+0/g, '');
  },
 
  div(variable) {
        return (numPoly, denomPoly) => {

            const numTerms = numPoly.split('+').map(term => term.trim());
            const [denomCoeff, denomPower] = this.splitTerm(denomPoly, variable);

            let result = numTerms.map(numTerm => {
                const [numCoeff, numPower] = this.splitTerm(numTerm, variable);

                const resultCoeff = numCoeff / denomCoeff;
                const resultPower = numPower - denomPower;

                if (resultPower === 0) {
                    return `${resultCoeff}`;
                } else if (resultPower > 0) {
                    return `${resultCoeff}${variable}${resultPower}`;
                } else {
                    return `${resultCoeff}/${variable}${Math.abs(resultPower)}`;
                }
            }).join('+');

            return result;
        };
    },

    splitTerm(term, variable) {
        if (!term.includes(variable)) return [parseFloat(term), 0];

        const [coeffPart, powerPart] = term.split(variable);
        const coeff = coeffPart === '' ? 1 : parseFloat(coeffPart || '1');
        const power = powerPart ? parseFloat(powerPart) : 1;

        return [coeff, power];
    },

};
/*
console.log(polynomialOperation.add('x')('x2+3', '2x+1')) // return "x2+2x+4"
console.log(polynomialOperation.sub('x')('x2+3', '2x+1')) // return "x2-2x+2"
console.log(polynomialOperation.mul('x')('x2+3', '2x+1')) // return "2x3+x2+6x+3"
console.log(polynomialOperation.dif('x')('x3+2x2+6'));  // return 3x2+4x1
console.log(polynomialOperation.cal('x')('(x2+3)+(2x+1)'));// return "x2+2x+4"
console.log(polynomialOperation.cal('x')('(x2+3)-(2x+1)'));// return "x2-2x+2"
console.log(polynomialOperation.cal('x')('(x2+3)(2x+1)'));// return "2x3+x2+6x+3"
console.log(polynomialOperation.dif('x')('(x2+3)(2x+1)'));// return "6x2+2x1+6"
console.log(polynomialOperation.div('x')('3x+3', '3x')); // return 1+1/x
console.log(polynomialOperation.add('x')('x1.5+3+2/x2.2', '2x+1')) // return "x1.5+2x+4+2/x2.2"
console.log(polynomialOperation.dif('x')('(x1.5+3x)(x+1)')) // return 2.5x1.5+6x1+1.5x0.5+3
*/
