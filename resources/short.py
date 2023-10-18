# Edwin Camuy, 2023-10-17
import sys
from math import lcm, ceil
from fractions import Fraction
  
def get_numeric_input(input_token: str) -> Fraction:
  res = Fraction(input_token).limit_denominator(16)
  if (res < 0):
    report_bad_usage("No negative values!")
  return res

def to_whole(frac: Fraction, conversion_factor: int) -> int:
  """
  Converts a fraction to an integer by multiplying it with a conversion factor.
  
  frac - base fraction
  conversion_factor - factor to multiply it by
  """
  return int(frac * conversion_factor)

def report_bad_usage(msg: str):
  print(msg)
  print("Usage: main.py <options>")
  print("""Options are
  -q: integer -> set a quanta duration in time steps
  -processes: list of strings -> list the names of the processes
  -durations: list of integers -> list the durations of the processes
  -entries: list of integers -> list the point in time where each process enters. default 0
  """)
  sys.exit()

def register_processes(names: list[str], durations: list[int], entries: list[int]) -> dict[str: tuple[int, int]]:
  """
  names - list of names of processes
  durations - list of durations
  entries - list of entries
  return - dict of processes. keys are process names, values are (duration, entry) tuples
  """

  if entries == []:
    entries = [0] * len(names)
    
  assert(len(names) == len(durations) == len(entries))
  assert(len(names) > 0)
  
  processes = {}

  for i in range(len(names)):
    processes[names[i]] = (durations[i], entries[i])

  return processes

def get_inputs(args: list[str]):
  """
  args - program arguments
  return - (quanta, processes)
  """
  q = Fraction(1, 1)
  q_been_set = False
  names = []
  durations = []
  entries = []

  all_denominators = []

  class InputMode:
    DEFAULT_MODE = 0
    Q_MODE = 1
    NAMES_MODE = 2
    DURATIONS_MODE = 3
    ENTRIES_MODE = 4

  mode = InputMode.DEFAULT_MODE

  for input_token in args[1::]:
    match input_token:
      case "-q":
        mode = InputMode.Q_MODE
      case "-processes":
        mode = InputMode.NAMES_MODE
      case "-durations":
        mode = InputMode.DURATIONS_MODE
      case "-entries":
        mode = InputMode.ENTRIES_MODE
      case _:
        if not input_token.isnumeric() and input_token[0] == '-':
          report_bad_usage(f"Unrecognized option {input_token}")
        match mode:
          case InputMode.DEFAULT_MODE:
            report_bad_usage("Must specify an option")
          case InputMode.Q_MODE:
            if q_been_set:
              report_bad_usage("Only one quanta duration can be specified")
            q = get_numeric_input(input_token)
            all_denominators.append(q.denominator)
            q_been_set = True
          case InputMode.NAMES_MODE:
            names.append(input_token)
          case InputMode.DURATIONS_MODE:
            durations.append(x := get_numeric_input(input_token))
            all_denominators.append(x.denominator)
          case InputMode.ENTRIES_MODE:
            entries.append(x := get_numeric_input(input_token))
            all_denominators.append(x.denominator)
          case _:
            report_bad_usage("Unknown option: " + input_token)
          
  conversion_factor = lcm(*all_denominators)
  adjusted_q = to_whole(q, conversion_factor)
  adjusted_durations = [to_whole(i, conversion_factor) for i in durations]
  adjusted_entries = [to_whole(i, conversion_factor) for i in entries]

  try:
    processes = register_processes(names, adjusted_durations, adjusted_entries)
    # If we have fractional parts we will need to convert them into integers through a conversion factor.
    return adjusted_q, processes, conversion_factor
  except AssertionError:
    report_bad_usage("The lists you sent did not match in length, or are empty")
    return None
  


def fcfs(processes):
  """
  processes - dictionary of processes
  return - list of timesteps with name of processes
  """
  res = []
  
  current_process = ""
  current_process_remaining = 0
  current_time = 0
  process_queue = sorted(processes.items(), key=lambda x: x[1][1])
  
  while process_queue:
    current_process, (current_process_remaining, entry) = process_queue.pop(0)
    if entry > current_time:
      # wait for process to enter queue
      res += [""] * (entry - current_time)
      current_time = entry

    res += [current_process] * current_process_remaining
    current_time += current_process_remaining    

  return res

def sjf(processes):
  """
  processes - dictionary of processes
  return - list of timesteps with name of processes
  """
  res = []

  process_queue = sorted(processes.items(), key=lambda x: x[1][0])
  process_queue.sort(key=lambda x: x[1][1])
  current_time = 0

  while process_queue:
    # Find shortest job that has arrived by now
    candidates = [i for i in process_queue if i[1][1] <= current_time]
    # if there are no candidates just pop and wait for the next one
    if candidates == []:
      current_process, (current_process_remaining, entry) = process_queue.pop(0)
    else:
      # Shortest job first
      i = candidates.index(min(candidates, key = lambda x: x[1][0]))
      current_process, (current_process_remaining, entry) = process_queue.pop(i)

    if entry > current_time:
      res += [""] * (entry - current_time)
      current_time = entry

    res += [current_process] * current_process_remaining
    current_time += current_process_remaining
    current_process_remaining = 0
  
  return res

def srjf(processes):
  """
  processes - dictionary of processes
  return - list of timesteps with name of processes
  """
  res = []

  have_entered = []
  have_not_entered = list(processes.keys())
  remaining_times = {k: v[0] for k, v in processes.items()}
  current_time = 0

  while any(remaining_times.values()):
    to_pop = []
    for i, process in enumerate(have_not_entered):
      if processes[process][1] <= current_time:
        have_entered.append(have_not_entered[i])
        to_pop.append(i)
    
    for i in to_pop[::-1]:
      have_not_entered.pop(i)

    if have_entered == []:
      res.append("")
    else:
      have_entered.sort(key=lambda x: remaining_times[x])
      current_process = have_entered[0]
      res.append(current_process)
      remaining_times[current_process] -= 1
      if remaining_times[current_process] == 0:
        have_entered.pop(0)
      
    current_time += 1

  return res

def rr(q, processes):
  """
  processes - dictionary of processes
  return - list of timesteps with name of processes
  """
  res = []

  remaining_q = q
  current_time = 0
  current_queue = []

  remaining_times = {k: v[0] for k, v in processes.items()}

  while any(remaining_times.values()):
    for process in processes.keys():
      if processes[process][1] == current_time:
        current_queue.append(process)
    
    if current_queue == []:
      res.append("")
    else:
      res.append(current_queue[0])
      remaining_times[current_queue[0]] -= 1
      if remaining_times[current_queue[0]] == 0:
        current_queue.pop(0)
    
    current_time += 1
    remaining_q -= 1
    if remaining_q == 0:
      if current_queue != []:
        current_queue.append(current_queue.pop(0))
      remaining_q = q

  return res

def get_streaks(arr: list) -> list:
  """
  Finds consecutive values in a list and returns a compressed list containing (value, streak) tuples
  
  arr - list
  return - list of (value, streak) tuples where streak = number of times a value appeared consecutively
  """
  res = []

  current_val = None
  current_count = 0

  for i in arr:
    if i == current_val:
      current_count += 1
    elif current_val == None:
      current_val = i
      current_count = 1
    else:
      assert(current_count > 0)
      res.append((current_val, current_count))
      current_val = i
      current_count = 1
  
  res.append((current_val, current_count))

  return res

def format_table(arr, width, conversion_factor):
  """
  arr - list in the form [pname, pname, pname, ...]
  width - minimum width in columns of the table
  return - formatted gantt chart
  """
  streaks = get_streaks(arr)

  scale = width / (len(arr))

  middle_row = "|"
  for name, length in streaks:
    if name == "":
      name = "Wait"
    label = f"{name}: {length / conversion_factor: 0.2f}"
    segment_desired_width = ceil(length * scale)
    padding_length = segment_desired_width - 1 - len(label)
    padding = " " * padding_length
    middle_row += label + padding + "|"
  middle_row += "\n"

  head = "+" + "-" * (len(middle_row) - 3) + "+\n"

  res = head + middle_row + head

  return res

def stats(arr, processes, conversion_factor):
  """
  arr - list in the form [pname, pname, pname, ...]
  processes - process dictionary
  conversion_factor - divides final measures to adjust
  return - (average turnaround, average waiting time)
  """

  turnarounds = {k: 0 for k in processes.keys()}
  waiting_times = {k: 0 for k in processes.keys()}
  response_times = {k: 0 for k in processes.keys()}

  remaining_times = {k: v[0] for k, v in processes.items()}
  has_started = {k: False for k in processes.keys()}

  for time, pname in enumerate(arr):
    if pname == "":
      continue

    has_started[pname] = True
    
    for k, v in processes.items():
      if remaining_times[k] != 0 and time >= v[1]:
        turnarounds[k] += 1
        if pname != k:
          waiting_times[k] += 1
        if not has_started[k]:
          response_times[k] += 1
    
    remaining_times[pname] -= 1

  avg_turnaround = sum(turnarounds.values()) / len(processes) / conversion_factor
  avg_waiting_time = sum(waiting_times.values()) / len(processes) / conversion_factor
  avg_response_time = sum(response_times.values()) / len(processes) / conversion_factor
  
  return avg_turnaround, avg_waiting_time, avg_response_time


def main():
  q, processes, conversion_factor = get_inputs(sys.argv)

  fcfs_arr = fcfs(processes)
  sjf_arr = sjf(processes)
  srjf_arr = srjf(processes)
  rr_arr = rr(q, processes)

  table_width = 100

  print("First Come First Serve")
  print(format_table(fcfs_arr, table_width, conversion_factor))
  turnaround, waiting_time, response_time = stats(fcfs_arr, processes, conversion_factor)
  print(f"Average turnaround: {turnaround:0.2f}")
  print(f"Average waiting time: {waiting_time:0.2f}")
  print(f"Average response time: {response_time:0.2f}\n\n")
  
  print("Shortest Job First")
  print(format_table(sjf_arr, table_width, conversion_factor))
  turnaround, waiting_time, response_time = stats(sjf_arr, processes, conversion_factor)
  print(f"Average turnaround: {turnaround:0.2f}")
  print(f"Average waiting time: {waiting_time:0.2f}")
  print(f"Average response time: {response_time:0.2f}\n\n")

  print("Shortest Remaining Job First")
  print(format_table(srjf_arr, table_width, conversion_factor))
  turnaround, waiting_time, response_time = stats(srjf_arr, processes, conversion_factor)
  print(f"Average turnaround: {turnaround:0.2f}")
  print(f"Average waiting time: {waiting_time:0.2f}")
  print(f"Average response time: {response_time:0.2f}")

  print(f"Round Robin q = {q // conversion_factor}")
  print(format_table(rr_arr, table_width, conversion_factor))
  turnaround, waiting_time, response_time = stats(rr_arr, processes, conversion_factor)
  print(f"Average turnaround: {turnaround:0.2f}")
  print(f"Average waiting time: {waiting_time:0.2f}")
  print(f"Average response time: {response_time:0.2f}\n\n")


if __name__ == "__main__":
  main()