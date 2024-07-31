import subprocess
import sys
import os
import shutil
import filecmp

def run(args):
    print('+ ' + ' '.join(args))
    return subprocess.run(args)

def testFace(face):
    global sourceDir, binDir, resultDir
    resultFile = resultDir + '/' + face + '.jpg'
    res = run([
        binDir + '/src/pano-projector',
        'face',
        '--face=' + face,
        sourceDir + '/tests/data/input/bass.jpg',
        resultFile])

    if res.returncode:
        print("pano-projector exited with return code %d" % res.returncode)
        return False

    expectFile = sourceDir + '/tests/data/expected/' + face + '.jpg'
    if not filecmp.cmp(resultFile, expectFile):
        print("File comparison mismatch in face " + face)
        return False

    return True

def testPyramid():
    global sourceDir, binDir, resultDir
    res = run([
        binDir + '/src/pano-projector',
        'pyramid',
        '--tile-size=128',
        sourceDir + '/tests/data/input/bass.jpg',
        resultDir])

    if res.returncode:
        print("pano-projector exited with return code %d" % res.returncode)
        return False

    res = True
    for face in ["b", "l", "f", "r", "u", "d"]:
        for level in [1, 2]:
            n = 1 if level == 1 else 2
            for x in range(0, n):
                for y in range(0, n):
                    rel = '%d/%s%d_%d.jpg' % (level, face, x, y)
                    expect = '%s/tests/data/expected/%s' % (sourceDir, rel)
                    result = '%s/%s' % (resultDir,rel)
                    if not filecmp.cmp(result, expect):
                        print("File comparison mismatch in file " + rel)
                        res = False

    return res

def main():
    global sourceDir, binDir, resultDir

    sourceDir = sys.argv[1]
    binDir = sys.argv[2]
    resultDir = binDir + '/test-result'

    if os.path.exists(resultDir):
        shutil.rmtree(resultDir)
    os.mkdir(resultDir)

    success = True
    for face in ["b", "l", "f", "r", "u", "d"]:
        if (testFace(face)):
            print("Face " + face + ": OK")
        else:
            print("Face " + face + ": FAILED")
            success = False

    if (testPyramid()):
        print("Pyramid: OK")
    else:
        print("Pyramid: FAILED")
        success = False

    sys.exit(0 if success else 1)

main()